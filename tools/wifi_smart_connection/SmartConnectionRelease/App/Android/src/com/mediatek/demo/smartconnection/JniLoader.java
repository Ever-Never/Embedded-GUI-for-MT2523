package com.mediatek.demo.smartconnection;

public class JniLoader {

    
    public static final int ERROR_CODE_OK                     = 0;
    public static final int ERROR_CODE_NOT_INITINED           = -1;
    public static final int ERROR_CODE_WRONG_TYPE             = -2;
    public static final int ERROR_CODE_WRONG_PARAMETER        = -3;
    public static final int ERROR_CODE_CRYPTED_FAILED         = -4;
    public static final int ERROR_CODE_NOT_ENOUGH_MEMORY      = -5;
    public static final int ERROR_CODE_EXCEED_MAX_LENGTH      = -6;
    public static final int ERROR_CODE_ALREADY_STARTED        = -7;
    public static final int ERROR_CODE_ALREADY_STOPED         = -8;
    public static final int ERROR_CODE_FAILED_TO_START        = -9;
    
    public JniLoader() {
        
    }
    
    public static boolean LoadLib() {
        try {
            System.loadLibrary("smart_connection_jni");
            return true;
        } catch (Exception ex) {
            System.err.println("WARNING: Could not load library");
            return false;
        }
    }

    public native int GetProtoVersion();
    public native int GetLibVersion();

    /**
     * Init SmartConnection
     */
    public native int InitSmartConnection(String key, String Target, int sendV1, int sendV4, int sendV5);
    
    /**
     * Set Send interval
     * @param interval
     * @return
     */
    public native int SetSendInterval(float oldInterval, float newInterval);

    /**
     * Start SmartConnection with Home AP
     *
     * @SSID : SSID of Home AP
     * @Password : Password of Home AP
     */
    public native int StartSmartConnection(String SSID, String Password, String Custom);

    /**
     * Stop SmartConnection by user
     *
     */

    public native int StopSmartConnection();
    
}
