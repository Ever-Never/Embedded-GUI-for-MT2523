
#include <stdlib.h>
#include <elian.h>
#include "smart_connection_jni_loader.h"


#define SEND_V1            0x01
#define SEND_V4            0x02
#define SEND_V5            0x04


void *context;

/*
 * Class:     com_mediatek_demo_smartconnection_JniLoader
 * Method:    GetProtoVersion
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_mediatek_demo_smartconnection_JniLoader_GetProtoVersion
  (JNIEnv *env, jobject object)
{
    int proVersion = 0;
    int libVersion = 0;
    elianGetVersion(&proVersion, &libVersion);
    return proVersion; 
}

/*
 * Class:     com_mediatek_demo_smartconnection_JniLoader
 * Method:    GetLibVersion
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_mediatek_demo_smartconnection_JniLoader_GetLibVersion
  (JNIEnv *env, jobject object)
{
    int proVersion = 0;
    int libVersion = 0;
    elianGetVersion(&proVersion, &libVersion);
    return libVersion; 
}

/*
 * Class:     com_mediatek_demo_smartconnection_JniLoader
 * Method:    InitSmartConnection
 * Signature: (Ljava/lang/String;III)I
 */
JNIEXPORT jint JNICALL Java_com_mediatek_demo_smartconnection_JniLoader_InitSmartConnection
  (JNIEnv *env, jobject object, jstring key, jstring tag, jint sendV1, jint sendV4, jint sendV5)
{
    unsigned char   target[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned int    flag = 0;
    const char      *encryp_key = NULL;

    if (context)
    {
	    elianStop(context);
	    elianDestroy(context);
	    context = NULL;
    }

    if (sendV1)
    {
    	flag |= SEND_V1;
    }
    if (sendV4)
    {
    	flag |= SEND_V4;
    }
    if (sendV5) {
        flag |= SEND_V5;
    }
    encryp_key = (*env)->GetStringUTFChars(env, key, 0);
    if (encryp_key != NULL && strlen(encryp_key) == 16) {
        context = elianNew(encryp_key, strlen(encryp_key), target, flag);
    } else {
        context = elianNew(NULL, 0, target, flag);
    }
    (*env)->ReleaseStringUTFChars(env, key, encryp_key);
    if (context == NULL)
    {
        return ELIAN_ERROR_CODE_NOT_INITED;
    }
    //elianSetInterval(context, 20*1000);
    return ELIAN_ERROR_CODE_OK;
}

/*
 * Class:     com_mediatek_demo_smartconnection_JniLoader
 * Method:    SetSendInterval
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_mediatek_demo_smartconnection_JniLoader_SetSendInterval
  (JNIEnv *env, jobject object, jfloat oldInterval, jfloat newInterval)
{
    unsigned int ointerval = 20*1000;
    unsigned int ninterval = 0;
    if (context == NULL) {
        return ELIAN_ERROR_CODE_NOT_INITED;
    }

    if (oldInterval  != 0) {
        ointerval = oldInterval * 1000;
    }
    if (newInterval != 0) {
        ninterval = newInterval * 1000;
    }
    return elianSetInterval(context, ointerval, ninterval);
}


/*
 * Class:     com_mediatek_demo_smartconnection_JniLoader
 * Method:    StartSmartConnection
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_mediatek_demo_smartconnection_JniLoader_StartSmartConnection
  (JNIEnv *env, jobject object, jstring SSID, jstring PASSWORD, jstring CUSTOM)
{
    const char *ssid = NULL;
    const char *password = NULL;
    const char *custom = NULL;
    int retValue = ELIAN_ERROR_CODE_OK;

    if (context == NULL)
    {
    	return ELIAN_ERROR_CODE_NOT_INITED;
    }
    ssid = (*env)->GetStringUTFChars(env, SSID, 0);
    password = (*env)->GetStringUTFChars(env, PASSWORD, 0);
    custom = (*env)->GetStringUTFChars(env, CUSTOM, 0);

//    elianPut(mcContext, TYPE_ID_AM, (char *)&authmode, 1);
    retValue = elianPut(context, TYPE_ID_SSID, (char *)ssid, strlen(ssid));
    if (retValue != ELIAN_ERROR_CODE_OK) {
        goto error;
    }
    elianPut(context, TYPE_ID_PWD, (char *)password, strlen(password));
    if (retValue != ELIAN_ERROR_CODE_OK) {
        goto error;
    }
    elianPut(context, TYPE_ID_CUST, (char *)custom, strlen(custom));
    if (retValue != ELIAN_ERROR_CODE_OK) {
        goto error;
    }

error:
    (*env)->ReleaseStringUTFChars(env, SSID, ssid);
    (*env)->ReleaseStringUTFChars(env, PASSWORD, password);
    (*env)->ReleaseStringUTFChars(env, CUSTOM, custom);

    elianStart(context);

    return retValue;
}

/*
 * Class:     com_mediatek_demo_smartconnection_JniLoader
 * Method:    StopSmartConnection
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_mediatek_demo_smartconnection_JniLoader_StopSmartConnection
  (JNIEnv *env, jobject object)
{
    if (context)
	{
	    elianStop(context);
	    elianDestroy(context);
	    context = NULL;
	}
    return ELIAN_ERROR_CODE_OK;
}



