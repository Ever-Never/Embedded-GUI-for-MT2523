Smart Connection 

=================================================================================
Introduction:

elian.h
    Header files which declared the APIs and static parameters

Folder Structure:
    App (The demo application for smart connection, including Android and iOS)
      |- Android    For eclipse project
      |- iOS

    jni (Only for Android application)
      It's a reference design, which including JNI class and C implementation.
      Prefer customize the JNI class and C implementation, if yes, please modify the Android.mk.
      
      How to build (Please use NDK and configure the NDK_PATH firstly):
      1. makedir static_lib
      2. Copy the folders under /libs/Android into the folder /jni/static_lib
      3. sh BuildSharedLibrary.sh

    libs
      |- Android    For all platforms (Static libraries which need be linked into jni mk)
      |- iOS        For arm/i386/x86_64 library (Link the library into UI project)


Attention:
    1. void *elianNew(const char *key, int keylen, const unsigned char *target, unsigned int flag);
        The flag should be ELIAN_VERSION_V5 or mixed with V1/V4 which defined in elian.h
        
        #define ELIAN_VERSION_V1            0x01
        #define ELIAN_VERSION_V4            0x02
        #define ELIAN_VERSION_V5            0x04
        
        e.g: flag = ELIAN_VERSION_V5 | ELIAN_VERSION_V4
    
    2. int elianSetInterval(void *context, unsigned int ousec, unsigned int nusec);
        Set the send interval, the unit is u_sec.
        ousec is for ELIAN_VERSION_V1 or ELIAN_VERSION_V4
        nusec is for ELIAN_VERSION_V5
        
    3. Run the demo project, if failed to receive the data, please increase the send interval and try again.
    
    4. In iOS, cause the library used the STL feature, please modify the file suffix from .m to .mm which one used the library.
=================================================================================


=================================================================================

Release Date : 2017/2/24
Initial version of the smart connection which support both V1/V4 and V5

=================================================================================

=================================================================================

Release Data : 2017/4/5
Change List:
1. Fix send failed issue in some platform. (sendto return -1 and errno is 14)

=================================================================================

=================================================================================

Release Data : 2017/4/5
Change List:
1. Fix encryption key length issue
    If the encryption key length is not 16, use the default encryption key

=================================================================================

=================================================================================

Release Data : 2017/4/17
Change List:
1. For Application
    Add encryption key input window, can exchange the encryption key.
    Meanwhile, the encryption key should change the encryption key. 
    Otherwise the smart connection will fail.

=================================================================================
    