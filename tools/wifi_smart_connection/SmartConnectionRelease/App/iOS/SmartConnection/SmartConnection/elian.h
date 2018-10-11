

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

#endif  //WIN32


/* Error Codes */
#define ELIAN_ERROR_CODE_OK                         0
#define ELIAN_ERROR_CODE_NOT_INITED                 -1
#define ELIAN_ERROR_CODE_WRONG_TYPE                 -2
#define ELIAN_ERROR_CODE_WRONG_PARAMETER            -3
#define ELIAN_ERROR_CODE_CRYPTED_FAILED             -4
#define ELIAN_ERROR_CODE_NOT_ENOUGH_MEMORY          -5
#define ELIAN_ERROR_CODE_EXCEED_MAX_LENGTH          -6
#define ELIAN_ERROR_CODE_ALREADY_STARTED            -7
#define ELIAN_ERROR_CODE_ALREADY_STOPED             -8
#define ELIAN_ERROR_CODE_FAILED_TO_START            -9

#define ELIAN_VERSION_V1            0x01
#define ELIAN_VERSION_V4            0x02
#define ELIAN_VERSION_V5            0x04


/* Data Type To Put */
enum etype_id {

    TYPE_ID_BEGIN = 0x0,
	TYPE_ID_AM,
	TYPE_ID_SSID,
	TYPE_ID_PWD,
	TYPE_ID_USER,
	TYPE_ID_PMK,
	TYPE_ID_CUST = 0x7F,
	TYPE_ID_MAX = 0xFF

};


/* For Different Platform to print logs */
//typedef (void )(*logPrinter)(...) printLogMethod;

#ifdef __cplusplus
extern "C" {
#endif

//Public APIs

/* For Public APIs */
//return context on success, NULL on fail
ELIAN_API void elianGetVersion(int *protoVersion, int *libVersion);
ELIAN_API void *elianNew(const char *key, int keylen, const unsigned char *target, unsigned int flag);
ELIAN_API int elianSetInterval(void *context, unsigned int ousec, unsigned int nusec);   //set send interval, in microsec
ELIAN_API int elianPut(void *context, enum etype_id id, char *buf, int len);
ELIAN_API int elianStart(void *context);
ELIAN_API void elianStop(void *context);
ELIAN_API void elianDestroy(void *context);





#ifdef __cplusplus
};
#endif

#endif /* __ELIAN_H__ */



