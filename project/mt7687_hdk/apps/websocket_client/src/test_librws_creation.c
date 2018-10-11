/*
 *   Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include <stdlib.h>
#include <assert.h>
#include <string.h>

#if defined(CMAKE_BUILD)
#undef CMAKE_BUILD
#endif


#if defined(XCODE)
#include "librws.h"
#else
#include <librws.h>
#endif

#include "rws_memory.h"

#include "test_librws_creation.h"

#if defined(CMAKE_BUILD)
#undef CMAKE_BUILD
#endif


#define WEBSOCKET_CONNECTED (0x01)
#define WEBSOCKET_DISCONNECTED (0x02)
#define WEBSOCKET_DATA_NOT_RECVED (0x04)

#ifdef MTK_WEBSOCKET_SSL_ENABLE
#define ECHO_WEBSOCKET_CER												\
"-----BEGIN CERTIFICATE-----\r\n"                                       \
"MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx\r\n"	\
"EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT\r\n"	\
"EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp\r\n"	\
"ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz\r\n"	\
"NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH\r\n"	\
"EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE\r\n"	\
"AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw\r\n"	\
"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD\r\n"	\
"E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH\r\n"	\
"/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy\r\n"	\
"DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh\r\n"	\
"GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR\r\n"	\
"tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA\r\n"	\
"AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE\r\n"	\
"FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX\r\n"	\
"WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu\r\n"	\
"9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr\r\n"	\
"gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo\r\n"	\
"2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO\r\n"	\
"LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI\r\n"	\
"4uJEvlz36hz1\r\n"                                                      \
"-----END CERTIFICATE-----\r\n"

static const char echo_websocket_cer[] = ECHO_WEBSOCKET_CER;
#endif

static rws_socket _socket = NULL;
static int state_flags = 0;

static void on_socket_received_text(rws_socket socket, const char *text, const unsigned int length)
{
	char *buff = NULL;

    if (!socket || !text || !length)
    {
        LOG_I(websocket_example, "%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    buff = (char *)rws_malloc(length + 1);
    if (!buff)
    {
        LOG_I(websocket_example, "%s: Not enough memory. len:%d", __FUNCTION__, length + 1);
        return;
    }

    state_flags &= (~WEBSOCKET_DATA_NOT_RECVED);

	memcpy(buff, text, length);
	buff[length] = 0;

	LOG_I(websocket_example, "%s: Wsoc Data received: %s", __FUNCTION__, buff);
    rws_free(buff);
    buff = NULL;
}

static void on_socket_received_bin(rws_socket socket, const void * data, const unsigned int length)
{
	char *buff = NULL;

    if (!socket || !data || !length)
    {
        LOG_I(websocket_example, "%s: Invalid parameter(s).", __FUNCTION__);
        return;
    }

    buff = (char *)rws_malloc(length + 1);
    if (!buff)
    {
        LOG_I(websocket_example, "%s: Not enough memory. len:%d", __FUNCTION__, length + 1);
        return;
    }

    state_flags &= ~WEBSOCKET_DATA_NOT_RECVED;

	memcpy(buff, data, length);
	buff[length] = 0;

	LOG_I(websocket_example, "%s: Wsoc Data received: %s", __FUNCTION__, buff);
    rws_free(buff);
    buff = NULL;
}

static void on_socket_connected(rws_socket socket)
{
	const char * test_send_text =
	"{\"version\":\"1.0\",\"supportedConnectionTypes\":[\"websocket\"],\"minimumVersion\":\"1.0\",\"channel\":\"/meta/handshake\"}";

	LOG_I(websocket_example, "%s: Wsoc Connected", __FUNCTION__);

    state_flags |= WEBSOCKET_CONNECTED;
    state_flags &= ~WEBSOCKET_DISCONNECTED;
	rws_socket_send_text(socket, test_send_text);
}

static void on_socket_disconnected(rws_socket socket)
{
	rws_error error = rws_socket_get_error(socket);
	if (error)
	{
        LOG_I(websocket_example, "%s: Socket disconnect with code, error: %i, %s",
              __FUNCTION__,
			  rws_error_get_code(error),
			  rws_error_get_description(error));
	}

    LOG_I(websocket_example, "%s: Wsoc Disconnected", __FUNCTION__);

    state_flags &= ~WEBSOCKET_CONNECTED;
    state_flags |= WEBSOCKET_DISCONNECTED;
    _socket = NULL;
}


int websoc_cli_test_int(const char *scheme, const char *host,
                            const char *path, const int port,
                            const char *cert)
{
    int sleep_count = 0;

    if (!scheme || !host || !path)
    {
        LOG_I(websocket_example, "%s: Invalid parameter(s).", __FUNCTION__);
        return -1;
    }

    if (_socket)
    {
        LOG_I(websocket_example, "%s: Socket is not closed.", __FUNCTION__);
        return -2;
    }

	_socket = rws_socket_create(); // create and store socket handle
	assert(_socket);

    state_flags = 0;
    state_flags |= WEBSOCKET_DATA_NOT_RECVED;

	rws_socket_set_scheme(_socket, scheme);
	rws_socket_set_host(_socket, host);
	rws_socket_set_path(_socket, path);
	rws_socket_set_port(_socket, port);

#ifdef MTK_WEBSOCKET_SSL_ENABLE
    if (cert)
    {
        rws_socket_set_server_cert(_socket, cert, strlen(cert) + 1);
    }
#endif

	rws_socket_set_on_disconnected(_socket, &on_socket_disconnected);
	rws_socket_set_on_connected(_socket, &on_socket_connected);
	rws_socket_set_on_received_text(_socket, &on_socket_received_text);
	rws_socket_set_on_received_bin(_socket, &on_socket_received_bin);

#if !defined(RWS_APPVEYOR_CI)
	// Connection denied for client applications
	rws_socket_connect(_socket);
#endif

    /* Connecting */
    while ((!(state_flags & WEBSOCKET_CONNECTED)) &&
           (!(state_flags & WEBSOCKET_DISCONNECTED)))
    {
        LOG_I(websocket_example, "wait for connection. loop time:%d.", sleep_count);
        rws_thread_sleep(1000);
        sleep_count++;
        if (50 == sleep_count)
        {
            break;
        }
    }

    /* Receiving data */
    sleep_count = 0;
    if (state_flags & WEBSOCKET_CONNECTED)
    {
        while ((state_flags & WEBSOCKET_DATA_NOT_RECVED) && _socket &&
               (state_flags & WEBSOCKET_CONNECTED))
        {
            LOG_I(websocket_example, "wait for data. loop time:%d.", sleep_count);
            rws_thread_sleep(1000);
            sleep_count++;
            if (20 == sleep_count)
            {
                break;
            }
        }
    }

    if (_socket)
    {
        rws_socket_disconnect_and_release(_socket);
    }

    _socket = NULL;

    LOG_I(websocket_example, "state_flags:%x.", state_flags);
	return state_flags & WEBSOCKET_DATA_NOT_RECVED ? -3 : 0;
}


void websoc_cli_test_entry(void)
{
    int ret = 0, ssl_ret = 0;

    LOG_I(websocket_example, "Test client ws.");
    ret = websoc_cli_test_int("ws", "echo.websocket.org", "/", 80, NULL);

#ifdef MTK_WEBSOCKET_SSL_ENABLE
    LOG_I(websocket_example, "Test client wss.");
    ssl_ret = websoc_cli_test_int("wss", "echo.websocket.org", "/", 443, echo_websocket_cer);
#endif

    LOG_I(websocket_example, "Test set get.");
    websoc_set_get_test();

    if (0 == ret && 0 == ssl_ret)
    {
        LOG_I(websocket_example, "example project test success.");
    }
    else
    {
        LOG_I(websocket_example, "example project test failed. ret:%d, ssl_ret:%d", ret, ssl_ret);
    }
}

