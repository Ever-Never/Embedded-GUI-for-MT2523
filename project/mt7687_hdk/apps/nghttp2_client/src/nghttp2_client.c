/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2013, 2014 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "sys_init.h"
#include "wifi_api.h"
#include "task_def.h"
#include "syslog.h"

#include "httpclient.h"
#include "nghttp2.h"
#include "nghttp2_client.h"
#include "mbedtls/certs.h"
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/net.h"

/* Kernel includes. */
#define DEBUG_LEVEL 2

#define NGHTTP2_CLIENT_DEBUG 1
#define HTTP2_MAX_HOST_LEN (64)
#define NOT_SUPPORT_H2 (0x00FF)
#define BUF_SIZE   (1024 * 1)

#if NGHTTP2_CLIENT_DEBUG
log_create_module(nghttp2_client, PRINT_LEVEL_INFO);
#define NGHTTP2_DBG(fmt,arg...)   LOG_I(nghttp2_client, "[NGHTTP2Client]: "fmt,##arg)
#else
#define NGHTTP2_DBG(x, ...)
#endif

typedef struct _httpclient {
    int socket;                    /**< Socket id */
    int remote_port;               /**< Http or https port */
    int m_httpResponseCode;        /**< Response code */
    char *header;                  /**< Custom header */
    char *m_basicAuthUser;         /**< User name for basic authentication */
    char *m_basicAuthPassword;     /**< Password for basic authentication */
    mbedtls_ssl_context ssl;       /**< SSL context */
    mbedtls_net_context fd;        /**< SSL net context */
} httpclient;

struct Connection {
    mbedtls_ssl_context *ssl;
    nghttp2_session *session;
    /* WANT_READ if SSL/TLS connection needs more input; or WANT_WRITE
       if it needs more output; or IO_NONE. This is necessary because
       SSL/TLS re-negotiation is possible at any time. nghttp2 API
       offers similar functions like nghttp2_session_want_read() and
       nghttp2_session_want_write() but they do not take into account
       SSL/TSL connection. */
    int want_io;
};

struct URI {
    const char *host;
    /* In this program, path contains query component as well. */
    const char *path;
    size_t pathlen;
    const char *hostport;
    size_t hostlen;
    size_t hostportlen;
    uint16_t port;
};

struct Request {
    char *host;
    /* In this program, path contains query component as well. */
    char *path;
    /* This is the concatenation of host and port with ":" in
       between. */
    char *hostport;
    /* Stream ID for this request. */
    int32_t stream_id;
    uint16_t port;
};

typedef struct {
    char host[HTTP2_MAX_HOST_LEN];
    int port;
} http2_server_addr_t;

typedef struct {
    mbedtls_x509_crt cacertl;
    const char *trusted_ca_crt;
    mbedtls_x509_crl *ca_crl;
    const char *peer_cn;
    int (*f_confirm)(int);
} http2_verify_source_t;

typedef struct {
    int authmode;
    int (*f_random)(void *p_rng, unsigned char *output, size_t output_len);
    void (*f_debug)(void *ctx, int level, const char *file, int line, const char *str);
    int (*f_recv)(void *ctx, unsigned char *buf, size_t len);
    int (*f_send)(void *ctx, const unsigned char *buf, size_t len);
} http2_common_settings_t;

typedef struct {
    mbedtls_ssl_config conf;
    http2_common_settings_t common_settings;
    http2_verify_source_t verify_source;
} http2_ssl_custom_conf_t;

enum { IO_NONE, WANT_READ, WANT_WRITE };

#define MAKE_NV(NAME, VALUE)                                                   \
{                                                                            \
    (uint8_t *) NAME, (uint8_t *)VALUE, sizeof(NAME) - 1, sizeof(VALUE) - 1,   \
        NGHTTP2_NV_FLAG_NONE                                                   \
}

#define MAKE_NV_CS(NAME, VALUE)                                                \
{                                                                            \
    (uint8_t *) NAME, (uint8_t *)VALUE, sizeof(NAME) - 1, strlen(VALUE),       \
        NGHTTP2_NV_FLAG_NONE                                                   \
}

//static mbedtls_net_context nghttp2_socket;
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;

mbedtls_x509_crt_profile mbedtls_x509_crt_profile_myclient =
{
    /* Hashes from SHA-1 and above */
    //MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_RIPEMD160 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA224 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ),
    0xFFFFFFF, /* Any PK alg    */
    0xFFFFFFF, /* Any curve     */
    1024,
};

static const int nghttp2_ciphersuite[] =
{
    MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256,
    MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256,
    MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256,
    MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256,
    0
};


/**
* @brief          This is debug function.
* @param[in]    ctx: not using.
* @param[in]    level: not using.
* @param[in]    file: not using.
* @param[in]    line: not using.
* @param[in]    str: log context.
* @return       None.
*/
static void my_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    LOG_I(nghttp2_client, "[NGHTTP2Client]: %s\n", str);
}


/**
* @brief       This funtion print the result.
* @param[in]   verify_result: confirm result.
* @return      None.
*/
static int real_confirm(int verify_result)
{
#define VERIFY_ITEM(Result, Item, ErrMsg) \
    do { \
        if (((Result) & (Item)) != 0) { \
            NGHTTP2_DBG(ErrMsg); \
        } \
    } while (0)

    NGHTTP2_DBG("certificate verification result: 0x%02x\r\n", verify_result);
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_EXPIRED, "! fail ! server certificate has expired\r\n");
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_REVOKED, "! fail ! server certificate has been revoked\r\n");
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_CN_MISMATCH, "! fail ! CN mismatch\r\n");
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_NOT_TRUSTED, "! fail ! self-signed or not signed by a trusted CA\r\n");
    return 0;
}


/**
* @brief          This funtion copy string.
* @param[in]   s: source string.
* @param[in]   len: string length.
* @return        Returns copy of string |s| with the length |len|. The returned string is NULL-terminated.
 */
static char *strcopy(const char *s, size_t len)
{
    char *dst;
    dst = pvPortMalloc(len + 1);
    memcpy(dst, s, len);
    dst[len] = '\0';
    return dst;
}


/**
* @brief       The implementation of nghttp2_send_callback type. Here we write |data| with size |length|
*              to the network and return the number of bytes actually written. See the documentation of
 * nghttp2_send_callback for the details.
*              To set this callback to :type:`nghttp2_session_callbacks`, use
*              `nghttp2_session_callbacks_set_on_frame_send_callback()`.
* @param[in]   session: nghttp2 session.
* @param[in]   data: data which want to send.
* @param[in]   length: data length.
* @param[in]   flags: no using.
* @param[in]   user_data: user data.
* @return      Send data length.
 */
static ssize_t send_callback(nghttp2_session *session, const uint8_t *data,
                             size_t length, int flags, void *user_data)
{
    struct Connection *connection;
    int rv;
    connection = (struct Connection *)user_data;
    connection->want_io = IO_NONE;
    NGHTTP2_DBG("send_callback data begins %s * %d!\r\n", data, length);

    rv = mbedtls_ssl_write(connection->ssl, data, (int)length);
    NGHTTP2_DBG("send_callback data ends len = %d!\r\n", rv);
    if (rv <= 0) {
        if (rv == MBEDTLS_ERR_SSL_WANT_READ || rv == MBEDTLS_ERR_SSL_WANT_WRITE) {
            connection->want_io = (rv == MBEDTLS_ERR_SSL_WANT_READ ? WANT_READ : WANT_WRITE);
            rv = NGHTTP2_ERR_WOULDBLOCK;
        } else {
            rv = NGHTTP2_ERR_CALLBACK_FAILURE;
        }
    }
    return rv;
}



/**
* @brief      The implementation of nghttp2_recv_callback type. Here we read |data| from the network
*             and write them in |buf|. The capacity of |buf| is |length| bytes. Returns the number of
*             bytes stored in |buf|. See the documentation of nghttp2_recv_callback for the details.
*             To set this callback to :type:`nghttp2_session_callbacks`, use
*             `nghttp2_session_callbacks_set_on_frame_send_callback()`.
* @param[in]  session: nghttp2 session.
* @param[in]  buf: receive data buffer.
* @param[in]  length: data length.
* @param[in]  flags: no using.
* @param[in]  user_data: user data.
* @return     Received data length.
 */
static ssize_t recv_callback(nghttp2_session *session, uint8_t *buf,
                             size_t length, int flags, void *user_data)
{
    struct Connection *connection;
    int rv;

    connection = (struct Connection *)user_data;
    connection->want_io = IO_NONE;
    NGHTTP2_DBG("recv_callback begin\r\n");
    rv = mbedtls_ssl_read(connection->ssl, buf, (int)length - 1);
    buf[(int)length] = '\0';

    NGHTTP2_DBG("recv_callback len= %d\r\n", rv);
    if (rv < 0) {
        if (rv == MBEDTLS_ERR_SSL_WANT_READ || rv == MBEDTLS_ERR_SSL_WANT_WRITE) {
            connection->want_io = (rv == MBEDTLS_ERR_SSL_WANT_READ ? WANT_READ : WANT_WRITE);
            rv = NGHTTP2_ERR_WOULDBLOCK;
        } else {
            rv = NGHTTP2_ERR_CALLBACK_FAILURE;
        }
    } else if (rv == 0) {
        rv = NGHTTP2_ERR_EOF;
    }
    return rv;
}

/**
* @brief       Callback function invoked after the frame |frame| is sent.
*              To set this callback to :type:`nghttp2_session_callbacks`, use
*              `nghttp2_session_callbacks_set_on_frame_send_callback()`.
* @param[in]   session: nghttp2 session.
* @param[in]   frame: nghttp2 frame.
* @param[in]   user_data: The |user_data| pointer is the third argument passed in to the call to
*              `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return      The implementation of this function must return 0 if it succeeds.
*              If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*              and `nghttp2_session_mem_send()` functions immediately return :enum:
*              `NGHTTP2_ERR_CALLBACK_FAILURE`.
*/
static int on_frame_send_callback(nghttp2_session *session,
                                  const nghttp2_frame *frame,
                                  void *user_data)
{
    size_t i;
    NGHTTP2_DBG("on_frame_send_callback %d\n", frame->hd.type);
    switch (frame->hd.type) {
        case NGHTTP2_HEADERS:
            if (nghttp2_session_get_stream_user_data(session, frame->hd.stream_id)) {
                const nghttp2_nv *nva = frame->headers.nva;
                NGHTTP2_DBG("[INFO] C --------> S (HEADERS)\n");
                for (i = 0; i < frame->headers.nvlen; ++i) {
                    NGHTTP2_DBG("%s: %s\n", nva[i].name, nva[i].value);
                }
            }
            break;
        case NGHTTP2_RST_STREAM:
            NGHTTP2_DBG("[INFO] C ------> S (RST_STREAM)\n");
            break;
        case NGHTTP2_GOAWAY:
            NGHTTP2_DBG("[INFO] C -------> S (GOAWAY)\n");
            break;
    }
    return 0;
}


/**
* @brief       Callback function invoked by `nghttp2_session_recv()` and `nghttp2_session_mem_recv()` when a frame is received.
*              If frame is HEADERS or PUSH_PROMISE, the ``nva`` and ``nvlen``member of their data structure are always
*              ``NULL`` and 0 respectively.  The header name/value pairs are emitted via:type:`nghttp2_on_header_callback`
*              To set this callback to :type:`nghttp2_session_callbacks`, use`nghttp2_session_callbacks_set_on_frame_send_callback()`.
*              For HEADERS, PUSH_PROMISE and DATA frames, this callback may be called after stream is closed (see:type:
*              `nghttp2_on_stream_close_callback`).  The application should check that stream is still alive using its own stream
*              management or :func:`nghttp2_session_get_stream_user_data()`.
*              Only HEADERS and DATA frame can signal the end of incoming data. If ``frame->hd.flags & NGHTTP2_FLAG_END_STREAM``
*              is nonzero, the|frame| is the last frame from the remote peer in this stream.
*              This callback won't be called for CONTINUATION frames.
*              HEADERS/PUSH_PROMISE + CONTINUATIONs are treated as single frame.
* @param[in]   session: nghttp2 session.
* @param[in]   frame: nghttp2 frame.
* @param[in]   user_data: The |user_data| pointer is the third argument passed in to the call to
*              `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return      The implementation of this function must return 0 if it succeeds.
*              If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*              and `nghttp2_session_mem_send()` functions immediately return :enum:
*              `NGHTTP2_ERR_CALLBACK_FAILURE`.
*/
static int on_frame_recv_callback(nghttp2_session *session,
                                  const nghttp2_frame *frame,
                                  void *user_data)
{
    size_t i;
    NGHTTP2_DBG("on_frame_recv_callback %d\n", frame->hd.type);
    switch (frame->hd.type) {
        case NGHTTP2_HEADERS:
            if (frame->headers.cat == NGHTTP2_HCAT_RESPONSE) {
                const nghttp2_nv *nva = frame->headers.nva;
                struct Request *req;
                req = nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
                if (req) {
                    NGHTTP2_DBG("[INFO] C <--------- S (HEADERS)\n");
                    for (i = 0; i < frame->headers.nvlen; ++i) {
                        NGHTTP2_DBG("%s %s\r\n", nva[i].name, nva[i].value);
                    }
                }
            }
            break;
        case NGHTTP2_RST_STREAM:
            NGHTTP2_DBG("[INFO] C <--------- S (RST_STREAM)\n");
            break;
        case NGHTTP2_GOAWAY:
            NGHTTP2_DBG("[INFO] C <-------- S (GOAWAY)\n");
            break;
        case NGHTTP2_DATA:
            if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
                NGHTTP2_DBG("end stream flag\r\n");
            }
            break;
    }
    return 0;
}


/**
* @brief       Callback function invoked when the stream |stream_id| is closed.
*              We use this function to know if the response is fully received. Since we just fetch 1 resource in this program, after
*              the response is received, we submit GOAWAY and close the session.
* @param[in]   session: nghttp2 session.
* @param[in]   stream_id: stream id.
* @param[in]   error_code: The reason of closure.
*              Usually one of :enum:`nghttp2_error_code`, but that is not guaranteed.  The stream_user_data, which was specified in
*              `nghttp2_submit_request()` or `nghttp2_submit_headers()`, is still available in this function.
* @param[in]   user_data: The |user_data| pointer is the third argument passed in to the call to
*              `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return      The implementation of this function must return 0 if it succeeds.
*              If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*              and `nghttp2_session_mem_send()` functions immediately return :enum:
*              `NGHTTP2_ERR_CALLBACK_FAILURE`.
 */
static int on_stream_close_callback(nghttp2_session *session, int32_t stream_id,
                                    uint32_t error_code,
                                    void *user_data)
{
    struct Request *req;
    req = nghttp2_session_get_stream_user_data(session, stream_id);
    if (req) {
        int rv;
        rv = nghttp2_session_terminate_session(session, NGHTTP2_NO_ERROR);

        if (rv != 0) {
            NGHTTP2_DBG("stream close nghttp2_session_terminate_session\r\n");
        }
    }
    return 0;
}


/**
* @brief        Callback function invoked when a chunk of data in DATA frame is received.
*               The implementation of nghttp2_on_data_chunk_recv_callback type. We use this function to print the received response body.
* @param[in]    session: nghttp2 session.
* @param[in]    flags: no using.
* @param[in]    stream_id: the stream ID this DATA frame belongs to.
* @param[in]    data: receive data.
* @param[in]    len: data length.
* @param[in]    user_data: The |user_data| pointer is the third argument passed in to the call to
*               `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return       The implementation of this function must return 0 if it succeeds.
*               If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*               and `nghttp2_session_mem_send()` functions immediately return :enum:
*               `NGHTTP2_ERR_CALLBACK_FAILURE`.
 */
static int on_data_chunk_recv_callback(nghttp2_session *session,
                                       uint8_t flags, int32_t stream_id,
                                       const uint8_t *data, size_t len,
                                       void *user_data)
{
    struct Request *req;
    req = nghttp2_session_get_stream_user_data(session, stream_id);
    if (req) {
        NGHTTP2_DBG("[INFO] C <----------- S (DATA chunk)\n" "%lu bytes\n", (unsigned long int)len);
        //printf("data chunk %s\n", data);
    }
    return 0;
}


/**
* @brief       Callback function invoked when a header name/value pair is received.
*              The implementation of nghttp2_on_data_chunk_recv_callback type. We use this function to print the received response body.
* @param[in]   session: nghttp2 session.
* @param[in]   frame: nghttp2 frame.
* @param[in]   name: header name.
* @param[in]   namelen: length of header name.
* @param[in]   value: header value.
* @param[in]   valuelen: length of header value.
* @param[in]   flags: no using.
* @param[in]   user_data: The |user_data| pointer is the third argument passed in to the call to
*              `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return      The implementation of this function must return 0 if it succeeds.
*              If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*              and `nghttp2_session_mem_send()` functions immediately return :enum:
*              `NGHTTP2_ERR_CALLBACK_FAILURE`.
*/
static int on_header_callback(nghttp2_session *session,
                              const nghttp2_frame *frame, const uint8_t *name,
                              size_t namelen, const uint8_t *value,
                              size_t valuelen, uint8_t flags,
                              void *user_data)
{

    switch (frame->hd.type) {
        case NGHTTP2_HEADERS:
            if (frame->headers.cat == NGHTTP2_HCAT_RESPONSE) {
                /* Print response headers for the initiated request. */
                NGHTTP2_DBG("on header callback %s %d %s %d!\n", name, namelen, value, valuelen);
                break;
            }
    }
    return 0;
}


/**
* @brief        Called when nghttp2 library gets started to receive header block.
* @param[in]    session: nghttp2 session.
* @param[in]    frame: nghttp2 frame.
* @param[in]    user_data: The |user_data| pointer is the third argument passed in to the call to
*               `nghttp2_session_client_new()` or `nghttp2_session_server_new()`
* @return       The implementation of this function must return 0 if it succeeds.
*               If nonzero is returned, it is treated as fatal error and `nghttp2_session_send()`
*               and `nghttp2_session_mem_send()` functions immediately return :enum:
*               `NGHTTP2_ERR_CALLBACK_FAILURE`.
*/
static int on_begin_headers_callback(nghttp2_session *session,
                                     const nghttp2_frame *frame,
                                     void *user_data)
{
    switch (frame->hd.type) {
        case NGHTTP2_HEADERS:
            if (frame->headers.cat == NGHTTP2_HCAT_RESPONSE) {
                NGHTTP2_DBG("on begin headers callback stream ID=%d!\n", (int)frame->hd.stream_id);
            }
            break;
    }
    return 0;
}


/**
* @brief           Setup callback functions.
*                  nghttp2 API offers many callback functions, but most of them are optional. The send_callback is always required.
*                  Since we use nghttp2_session_recv(), the recv_callback is also required.
* @param[in|out]   callbacks: nghttp2 callbacks.
* @return          None.
 */
static void setup_nghttp2_callbacks(nghttp2_session_callbacks *callbacks)
{
    nghttp2_session_callbacks_set_send_callback(callbacks, send_callback);

    nghttp2_session_callbacks_set_recv_callback(callbacks, recv_callback);

    nghttp2_session_callbacks_set_on_frame_send_callback(callbacks,
            on_frame_send_callback);

    nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks,
            on_frame_recv_callback);

    nghttp2_session_callbacks_set_on_stream_close_callback(
        callbacks, on_stream_close_callback);

    nghttp2_session_callbacks_set_on_data_chunk_recv_callback(
        callbacks, on_data_chunk_recv_callback);

    nghttp2_session_callbacks_set_on_header_callback(callbacks,
            on_header_callback);

    nghttp2_session_callbacks_set_on_begin_headers_callback(
        callbacks, on_begin_headers_callback);

}


/**
* @brief         Submits the request |req| to the connection |connection|.
*                This function does not send packets; just append the request to the internal queue in |connection->session|.
* @param[in]     connection: ssl connection and nghttp2 session.
* @param[in]     req: request context.
* @return        None.
 */
static void submit_request(struct Connection *connection, struct Request *req)
{
    int32_t stream_id;
    /* Make sure that the last item is NULL */

    const nghttp2_nv nva[] = {MAKE_NV(":method", "GET"),
                              MAKE_NV_CS(":path", req->path),
                              MAKE_NV(":scheme", "https"),
                              MAKE_NV_CS(":authority", req->hostport),
                              MAKE_NV("accept", "*/*"),
                              MAKE_NV("user-agent", "nghttp2/" NGHTTP2_VERSION)
                             };

    stream_id = nghttp2_submit_request(connection->session, NULL, nva, sizeof(nva) / sizeof(nva[0]), NULL, req);

    if (stream_id < 0) {
        NGHTTP2_DBG("nghttp2_submit_request %d", (int)stream_id);
    }

    req->stream_id = stream_id;
    NGHTTP2_DBG("[INFO] Stream ID = %d\n", (int)stream_id);
}


/**
* @brief         Initation the requset with uri.
* @param[in]     req: Request.
* @param[in]     uri: The request's destination.
* @return        None.
*/
static void request_init(struct Request *req, const struct URI *uri)
{
    req->host = strcopy(uri->host, uri->hostlen);
    req->port = uri->port;
    req->path = strcopy(uri->path, uri->pathlen);
    req->hostport = strcopy(uri->hostport, uri->hostportlen);
    req->stream_id = -1;
}


/**
* @brief          Requst free.
* @param[in]      req: Request.
* @return         None.
*/
static void request_free(struct Request *req)
{
    vPortFree(req->host);
    vPortFree(req->path);
    vPortFree(req->hostport);
}


/**
* @brief         Pares URL to host.
* @param[in]     url: destination url.
* @param[in]     host. destination host.
* @param[in]     maxHostLen: Maximun length of host.
* @return        None.
*/
static int nghttp2_parseURL_host(char *url, char *host, size_t maxHostLen)   //Parse URL
{
    size_t hostLen = 0;
    char *hostPtr = NULL; 
    char *pathPtr = NULL;
    
    hostPtr = (char *) strstr(url, "://");
    if (hostPtr == NULL) {
        NGHTTP2_DBG("Could not find host");
        return HTTPCLIENT_ERROR_PARSE; //URL is invalid
    }
    hostPtr += 3;

    pathPtr = strchr(hostPtr, '/');
    hostLen = pathPtr - hostPtr;

    if (maxHostLen < hostLen +1 ) { //including NULL-terminating char
        NGHTTP2_DBG("Host str is too small (%d >= %d)", maxHostLen, hostLen + 1);
        return HTTPCLIENT_ERROR_PARSE;
    }
    memcpy(host, hostPtr, hostLen);
    host[hostLen] = '\0';

    return HTTPCLIENT_OK;
}

/**
* @brief         Initation the SSL client.
* @param[in]     ssl: mbedtls ssl struct.
* @param[in]     tcp_fd. The underlying file descriptor.
* @param[in]     custom_config: custome config.
* @return        The result. 0 is ok.
*/
static int nghttp2_ssl_client_init(mbedtls_ssl_context *ssl,
                                   mbedtls_net_context *tcp_fd,
                                   http2_ssl_custom_conf_t *custom_config)
{
    int ret = -1;
    const char *pers = "nghttp2_client";

    http2_verify_source_t *verify_source = &custom_config->verify_source;
    mbedtls_ssl_config *conf = &(custom_config->conf);

    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_net_init(tcp_fd);
    mbedtls_ssl_init(ssl);
    mbedtls_ssl_config_init(conf);

    mbedtls_x509_crt_init(&(verify_source->cacertl));
    mbedtls_ctr_drbg_init(&ctr_drbg);

    NGHTTP2_DBG("\n  . Seeding the random number generator...");

    mbedtls_entropy_init(&entropy);
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                        (const unsigned char *)pers,
                                        strlen(pers))) != 0 ) {
        NGHTTP2_DBG(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        return ret;
    }

    NGHTTP2_DBG(" ok\n");

    /*
     * 0. Initialize certificates
     */

    NGHTTP2_DBG("  . Loading the CA root certificate ...");

    if (NULL != verify_source->trusted_ca_crt) {
        if (0 != (ret = mbedtls_x509_crt_parse(&verify_source->cacertl,
                                               (unsigned char *)verify_source->trusted_ca_crt,
                                               strlen(verify_source->trusted_ca_crt) + 1))) {
            NGHTTP2_DBG(" failed ! x509parse_crt returned -0x%04x", -ret);
            return ret;
        }
    }

    NGHTTP2_DBG(" ok (%d skipped)", ret);

    return 0;
}


/**
* @brief          Start the SSL client.
* @param[in]      pssl: mbedtls ssl struct.
* @param[in]      psocket. The underlying file descriptor.
* @param[in]      server_addr. server address.
* @param[in]      custom_config: custome config.
* @return         The result. 0 is ok.
*/
static int nghttp2_ssl_start(mbedtls_ssl_context *pssl,
                             mbedtls_net_context *psocket,
                             http2_server_addr_t *server_addr,
                             http2_ssl_custom_conf_t *custom_config)
{

    /*
     * 0. Init
     */
    int ret = -1;


    /* alpn */
    const char *alpn_list[5];
    char a[] = "http/1.1";
    char b[] = "h2";
    char c[] = "h2-14";
    char d[] = "h2-16";

    alpn_list[0] = b;
    alpn_list[1] = c;
    alpn_list[2] = d;
    alpn_list[3] = a;
    alpn_list[4] = NULL;

    if (0 != (ret = nghttp2_ssl_client_init(pssl, psocket, custom_config))) {
        NGHTTP2_DBG(" failed ! nghttp2_ssl_client_init returned -0x%04x", -ret );
        return ret;
    }
    NGHTTP2_DBG("  . Connecting to tcp/%s/%4d...", server_addr->host, server_addr->port);

    /*
     * 1. Start the connection
     */
    if (0 != (ret = mbedtls_net_connect(psocket, server_addr->host, "443", MBEDTLS_NET_PROTO_TCP))) {
        NGHTTP2_DBG(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
    NGHTTP2_DBG(" ok");

    /*
     * 2. Setup stuff
     */
    NGHTTP2_DBG("  . Setting up the SSL/TLS structure..." );
    if ( ( ret = mbedtls_ssl_config_defaults( &(custom_config->conf),
                 MBEDTLS_SSL_IS_CLIENT,
                 MBEDTLS_SSL_TRANSPORT_STREAM,
                 MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 ) {
        NGHTTP2_DBG(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        return ret;
    }
    NGHTTP2_DBG( " ok" );

    /* alpn */
    if ((ret = mbedtls_ssl_conf_alpn_protocols(&(custom_config->conf), alpn_list)) != 0) {
        NGHTTP2_DBG("mbedtls_ssl_conf_alpn_protocols failed ret = %d\r\n", ret);
        return ret;
    }

    /* OPTIONAL is not optimal for security,
      * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode( &(custom_config->conf), custom_config->common_settings.authmode );

    mbedtls_ssl_conf_ca_chain( &(custom_config->conf), &(custom_config->verify_source.cacertl), custom_config->verify_source.ca_crl );

    mbedtls_ssl_conf_rng( &(custom_config->conf), custom_config->common_settings.f_random, &ctr_drbg );
    mbedtls_ssl_conf_dbg( &(custom_config->conf), custom_config->common_settings.f_debug, NULL );

    mbedtls_ssl_conf_cert_profile( &(custom_config->conf), &mbedtls_x509_crt_profile_myclient);
    mbedtls_ssl_conf_ciphersuites(&(custom_config->conf), (const int *)&nghttp2_ciphersuite);

    if ( ( ret = mbedtls_ssl_setup( pssl, &(custom_config->conf) ) ) != 0 ) {
        NGHTTP2_DBG(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        return ret;
    }

    /*
    * set host name, related with SNI
    */
    if ( ( ret = mbedtls_ssl_set_hostname(pssl, server_addr->host) ) != 0 ) {
        NGHTTP2_DBG( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        return ret;
    }

    mbedtls_ssl_set_bio( pssl, psocket, custom_config->common_settings.f_send, custom_config->common_settings.f_recv, mbedtls_net_recv_timeout );
    mbedtls_ssl_conf_read_timeout(&(custom_config->conf), 10000);
    mbedtls_ssl_conf_min_version(&(custom_config->conf), MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);

    /*
     * 4. Handshake
     */
    NGHTTP2_DBG(". Performing the SSL/TLS handshake...");

    while ((ret = mbedtls_ssl_handshake(pssl)) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            NGHTTP2_DBG(" failed  ! mbedtls_ssl_handshake returned -0x%04x", -ret);
            return ret;
        }
    }

    /*
    you can check if alpn_str is "h2", if not, should fallback to http/1.
    */
    const char *alpn_str = mbedtls_ssl_get_alpn_protocol(pssl);
    NGHTTP2_DBG("[ application layer protocol chosen is %s ]", alpn_str ? alpn_str : "(none)");
    if (alpn_str == NULL) {
        ret = NOT_SUPPORT_H2; //you can define an value you know
        return ret;
    }

    NGHTTP2_DBG(" ok");

    /*
     * 5. Verify the server certificate
     */
    NGHTTP2_DBG(" Verifying peer X.509 certificate...");
    http2_verify_source_t *verify_source = &custom_config->verify_source;
    if ((NULL != verify_source->f_confirm)
        && (0 != (ret = verify_source->f_confirm(mbedtls_ssl_get_verify_result(pssl))))) {
        NGHTTP2_DBG(" failed  ! verify result not confirmed.");
        return ret;
    }

    return 0;
}


/**
* @brief          Connect the SSL client.
* @param[in]      pclient: http client.
* @param[in]      url. destination url.
* @param[in]      port. destination port.
* @param[in]      ssl_config: custome config.
* @return         The result. 0 is ok.
*/
static int nghttp2s_client_conn(httpclient *pclient, char *url, int port, http2_ssl_custom_conf_t *ssl_config)
{    
    http2_server_addr_t server_addr;
    memset(&server_addr, 0, sizeof(server_addr));//ssl_config->common_settings.authmode = MBEDTLS_SSL_VERIFY_OPTIONAL;
    
    ssl_config->common_settings.authmode = MBEDTLS_SSL_VERIFY_NONE;
    ssl_config->common_settings.f_random = mbedtls_ctr_drbg_random;
    ssl_config->common_settings.f_debug  = my_debug;
    ssl_config->common_settings.f_recv   = mbedtls_net_recv;
    ssl_config->common_settings.f_send   = mbedtls_net_send;


    /* URL type: http[s]://host/ */
    if (HTTPCLIENT_ERROR_PARSE == nghttp2_parseURL_host(url, server_addr.host, HTTP2_MAX_HOST_LEN)) {
        NGHTTP2_DBG("Parse URL failed.");
        return -1;
    }
    server_addr.port = HTTPS_PORT;

    ssl_config->verify_source.trusted_ca_crt = NULL;
    ssl_config->verify_source.peer_cn = NULL;
    ssl_config->verify_source.ca_crl = NULL;
    ssl_config->verify_source.f_confirm = real_confirm;

    return nghttp2_ssl_start(&pclient->ssl, &pclient->fd, &server_addr, ssl_config);
}


/**
* @brief          Connect the nghttp2 client.
* @param[in]      pclient: http client.
* @param[in]      url. destination url.
* @param[in]      port. destination port.
* @param[in]      ssl_config: custome config.
* @param[in]      uri: destination uri.
* @return         The result. 0 is ok.
*/
static int nghttp2client_connect(httpclient *pclient, char *url, int port, http2_ssl_custom_conf_t *ssl_config, const struct URI *uri)
{
    struct Connection connection;
    nghttp2_session_callbacks *callbacks;
    int rv;
    int ret = 0;
    struct Request req;
    request_init(&req, uri);

    if (0 == (ret = nghttp2s_client_conn(pclient, url, port, ssl_config))) {
        pclient->remote_port = HTTPS_PORT;
        //nghttp2_socket.fd = pclient->fd.fd;
    } else {
        NGHTTP2_DBG("https_client_conn failed %d\r\n", ret);
        /* Resource cleanup */
        mbedtls_ssl_close_notify( &(pclient->ssl) );
        mbedtls_net_free( &pclient->fd );
        mbedtls_x509_crt_free( &(ssl_config->verify_source.cacertl) );
        mbedtls_ssl_free( &(pclient->ssl) );
        mbedtls_ssl_config_free( &(ssl_config->conf) );
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        request_free(&req);
        return ret;

    }

    connection.ssl = &(pclient->ssl);
    rv = nghttp2_session_callbacks_new(&callbacks);

    if (rv != 0) {
        NGHTTP2_DBG("nghttp2_session_callbacks_new1 %d", rv);
    }

    setup_nghttp2_callbacks(callbacks);
    rv = nghttp2_session_client_new(&connection.session, callbacks, &connection);

    nghttp2_session_callbacks_del(callbacks);

    if (rv != 0) {
        NGHTTP2_DBG("nghttp2_session_client_new2 %d", rv);
    }

    nghttp2_submit_settings(connection.session, NGHTTP2_FLAG_NONE, NULL, 0);

    /* Submit the HTTP request to the outbound queue. */
    submit_request(&connection, &req);

    /* Event loop */
    while (1) {
        int read_flag = 0;
        int write_flag = 0;

        write_flag = nghttp2_session_want_write(connection.session);
        if (write_flag) {
            int rv = nghttp2_session_send(connection.session);
            NGHTTP2_DBG("nghttp2_session_send %d\r\n", rv);
            if (rv < 0) {
                write_flag = 0;
            }
        }

        read_flag = nghttp2_session_want_read(connection.session);
        if (read_flag) {
            int rv = nghttp2_session_recv(connection.session);
            NGHTTP2_DBG("nghttp2_session_recv %d\r\n", rv);
            if (rv < 0) {
                read_flag = 0;
            }
        }

        NGHTTP2_DBG("write_flag = %d, read_flag = %d\r\n", write_flag, read_flag);

        if ((read_flag == 0) && (write_flag == 0)) {
            NGHTTP2_DBG("No active stream!\r\n");
            break;
        }
    }

    /* Resource cleanup */
    nghttp2_session_del(connection.session);

    mbedtls_ssl_close_notify( &(pclient->ssl) );
    mbedtls_net_free( &pclient->fd );
    mbedtls_x509_crt_free( &(ssl_config->verify_source.cacertl) );
    mbedtls_ssl_free( &(pclient->ssl) );
    mbedtls_ssl_config_free( &(ssl_config->conf) );
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    request_free(&req);
    return 0;

}


/**
* @brief          parse uri.
* @param[in]      URI: destination URI.
* @param[out]     uri. destination url.
* @return         The result. 0 is ok.
*/
static int parse_uri(struct URI *res, const char *uri)
{
    /* We only interested in https */
    size_t len, i, offset;
    int ipv6addr = 0;
    memset(res, 0, sizeof(struct URI));
    len = strlen(uri);
    if (len < 9 || memcmp("https://", uri, 8) != 0) {
        return -1;
    }
    offset = 8;
    res->host = res->hostport = &uri[offset];
    res->hostlen = 0;
    if (uri[offset] == '[') {
        /* IPv6 literal address */
        ++offset;
        ++res->host;
        ipv6addr = 1;
        for (i = offset; i < len; ++i) {
            if (uri[i] == ']') {
                res->hostlen = i - offset;
                offset = i + 1;
                break;
            }
        }
    } else {
        const char delims[] = ":/?#";
        for (i = offset; i < len; ++i) {
            if (strchr(delims, uri[i]) != NULL) {
                break;
            }
        }
        res->hostlen = i - offset;
        offset = i;
    }
    if (res->hostlen == 0) {
        return -1;
    }
    /* Assuming https */
    res->port = 443;
    if (offset < len) {
        if (uri[offset] == ':') {
            /* port */
            const char delims[] = "/?#";
            int port = 0;
            ++offset;
            for (i = offset; i < len; ++i) {
                if (strchr(delims, uri[i]) != NULL) {
                    break;
                }
                if ('0' <= uri[i] && uri[i] <= '9') {
                    port *= 10;
                    port += uri[i] - '0';
                    if (port > 65535) {
                        return -1;
                    }
                } else {
                    return -1;
                }
            }
            if (port == 0) {
                return -1;
            }
            offset = i;
            res->port = port;
        }
    }
    res->hostportlen = uri + offset + ipv6addr - res->host;
    for (i = offset; i < len; ++i) {
        if (uri[i] == '#') {
            break;
        }
    }
    if (i - offset == 0) {
        res->path = "/";
        res->pathlen = 1;
    } else {
        res->path = &uri[offset];
        res->pathlen = i - offset;
    }
    return 0;
}

/**
* @brief          Http1 client entry function.
* @param[in]      url_path: destination url.
* @return         None.
*/
static void http11client_test_entry(char* url_path)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf;

    /* Print a message to indicate the task has started. */
    NGHTTP2_DBG("http11client_test start.\r\n");

    buf = pvPortMalloc(BUF_SIZE);
    if (buf == NULL) {
        NGHTTP2_DBG("http11client_test malloc failed.\r\n");
        return;
    }
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;

    httpclient_get(&client, url_path, &client_data);

    NGHTTP2_DBG("http11client_test data received: %s\r\n", client_data.response_buf);
    vPortFree(buf);

    NGHTTP2_DBG("http11client_test end.\r\n");

}



/**
* @brief             Http2 client entry function.
* @return            None.
*/
void nghttp2client_test_entry(void)
{
    int mfs = 0;
    int ret = 0;
    struct URI uri;
    httpclient client = {0};
    http2_ssl_custom_conf_t ssl;

    char* url = "https://http2.akamai.com/";
    //char* url="https://www.ianlewis.org/";
    //char *url = "https://nghttp2.org/";

    NGHTTP2_DBG("nghttp2client_test_entry begin!\r\n");
    mfs = xPortGetFreeHeapSize();
    NGHTTP2_DBG("mfs 1 =%d\r\n", mfs);

    memset(&ssl, 0, sizeof(http2_ssl_custom_conf_t));

    parse_uri(&uri, url);
    ret = nghttp2client_connect(&client, url, HTTPS_PORT, &ssl, &uri);

    mfs = xPortGetFreeHeapSize();
    NGHTTP2_DBG("mfs 2 =%d\r\n", mfs);

    if(ret != 0)
    {
         NGHTTP2_DBG("server does not support h2 protocol, need fallback http/1.1\r\n");
         mfs = xPortGetFreeHeapSize();
         NGHTTP2_DBG("mfs 3 =%d\r\n", mfs);
         http11client_test_entry(url);
         mfs = xPortGetFreeHeapSize();
         NGHTTP2_DBG("mfs 4 =%d\r\n", mfs);
    }
    else
    {
        NGHTTP2_DBG("server is http/2 server!\r\n");
        NGHTTP2_DBG("example project test success.");
    }
    mfs = xPortGetFreeHeapSize();
    NGHTTP2_DBG("mfs 5 =%d\r\n", mfs);
    NGHTTP2_DBG("nghttp2client_test_entry end!\r\n");
}

