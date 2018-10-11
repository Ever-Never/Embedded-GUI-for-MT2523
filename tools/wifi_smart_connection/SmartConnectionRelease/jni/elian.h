

#ifndef __ELIAN_H__
#define __ELIAN_H__

#if defined(WIN32) || defined(WINAPI_FAMILY)

#ifdef ELIAN_EXPORTS
#define ELIAN_API __declspec(dllexport)
#else
#define ELIAN_API __declspec(dllimport)
#endif /* ELIAN_EXPORTS */

#else

#define ELIAN_API

#endif  //WIN32 | WINAPI_FAMILY


/**
 * Error Codes
 */
/* Operate Succeed */
#define ELIAN_ERROR_CODE_OK                         0
/* Operate failed, cause the Smart Connection Context is not inited */
#define ELIAN_ERROR_CODE_NOT_INITED                 -1
/* Operate failed, cause set the wrong type parameter */
#define ELIAN_ERROR_CODE_WRONG_TYPE                 -2
/* Operate failed, cause parameter is wrong */
#define ELIAN_ERROR_CODE_WRONG_PARAMETER            -3
/* Operate failed, cause encrypted failed */
#define ELIAN_ERROR_CODE_CRYPTED_FAILED             -4
/* Operate failed, cause not enough memory to put data */
#define ELIAN_ERROR_CODE_NOT_ENOUGH_MEMORY          -5
/* Operate failed, cause the parameter exceed the max length */
#define ELIAN_ERROR_CODE_EXCEED_MAX_LENGTH          -6
/* Operate failed, cause the Smart Connection already started */
#define ELIAN_ERROR_CODE_ALREADY_STARTED            -7
/* Operate failed, cause the Smart Connection already stopped */
#define ELIAN_ERROR_CODE_ALREADY_STOPED             -8
/* Operate failed, common error, failed to start */
#define ELIAN_ERROR_CODE_FAILED_TO_START            -9

/**
 * Version
 */
#define ELIAN_VERSION_V1            0x01
#define ELIAN_VERSION_V4            0x02
/* Recommended version */
#define ELIAN_VERSION_V5            0x04


/**
 * Enumeration defines the put type
 */
enum etype_id {

    TYPE_ID_BEGIN = 0x0,
	TYPE_ID_AM,             /* Authmode, Only for V1 and V4 */
	TYPE_ID_SSID,           /* WiFi AP SSID */
	TYPE_ID_PWD,            /* WiFi AP Password */
	TYPE_ID_USER,
	TYPE_ID_PMK,
	TYPE_ID_CUST = 0x7F,    /* Customer Data */
	TYPE_ID_MAX = 0xFF

};


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @bref Get protocol version and library version
 * @param[out] protoVersion     protocol version
 * @param[out] libVersion       library version
 */
ELIAN_API void elianGetVersion(int *protoVersion, int *libVersion);

/**
 * @bref New/Allocate a new Smart Connection Context
 * @param[in] key           The Encryption key, which used to encrypt the information, if the key is NULL, the library will use the pre-defined key
 * @param[in] keylen        The Encryption key length
 * @param[in] target        Which device to receive and handle the information, if the target is 0xff, all device will receive and hanele it
 * @param[in] flag          Version flags, e.g. V1 | V4, V5, V4 | V5
 * @return                  If succeed to allocate, return NOT NULL; otherwise, return NULL
 */
ELIAN_API void *elianNew(const char *key, int keylen, const unsigned char *target, unsigned int flag);

/**
 * @bref Set sending interval for each packet. If encrease the sending interval, it will cost more tiem to collect WiFi information
 * @param[in] context       The Smart Connection Context which is allocated in elianNew
 * @param[in] ousec         Only for version V1 and V4
 * @param[in] nusec         Only for version V5
 * @return                  If succeed to set interval, return ELIAN_ERROR_CODE_OK
 */
ELIAN_API int elianSetInterval(void *context, unsigned int ousec, unsigned int nusec);

/**
 * @bref Put parameters into Smart Connection Context which will be sent to the air.
 * @param[in] context       Pointer to Smart Connection Context
 * @param[in] id            Which type of the buf should be put into context
 * @param[in] buf           The content to put
 * @param[in] len           The content length
 * @return                  If succeed to put data, return ELIAN_ERROR_CODE_OK
 */
ELIAN_API int elianPut(void *context, enum etype_id id, char *buf, int len);

/**
 * @bref Start to send Smart Connection Packets into the air
 * @param[in] context       The pointer to Smart Connection Context
 * @return                  If succeed to start, return ELIAN_ERROR_CODE_OK
 */
ELIAN_API int elianStart(void *context);

/**
 * @bref Stop to send Smart Connection Packets
 * @param[in] context        The pointer to Smart Connection Context
 */
ELIAN_API void elianStop(void *context);

/**
 * @bref Destroy Smart Connection Context
 */
ELIAN_API void elianDestroy(void *context);


#ifdef __cplusplus
};
#endif

#endif /* __ELIAN_H__ */



