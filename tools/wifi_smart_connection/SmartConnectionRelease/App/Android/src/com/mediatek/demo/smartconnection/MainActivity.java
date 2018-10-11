package com.mediatek.demo.smartconnection;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.widget.TextView;


public class MainActivity extends Activity {

    private Button mStartButton;
    private Button mStopButton;
    private ProgressBar mProgressBar;
    
    private EditText mNameEdit;
    private EditText mPswEdit;
    private EditText mCustomInfoEdit;
    private EditText mEncryptionKeyEdit;
    
    private EditText mV1V4IntervalEdit;
    private EditText mV5IntervalEdit;

    private TextView mWatcher;
    
    private CheckBox mV1CheckBox;
    private CheckBox mV4CheckBox;
    private CheckBox mV5CheckBox;
    
    private Context mContext;

    private JniLoader loader;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        mStartButton = (Button)this.findViewById(R.id.btn_start);
        mStopButton = (Button)this.findViewById(R.id.btn_stop);
        mProgressBar = (ProgressBar)this.findViewById(R.id.sending_indicator);
        
        mNameEdit = (EditText)this.findViewById(R.id.et_name);
        mPswEdit = (EditText)this.findViewById(R.id.et_psw);
        mCustomInfoEdit = (EditText)this.findViewById(R.id.et_custom_info);
        mEncryptionKeyEdit = (EditText)this.findViewById(R.id.et_encryption_key);
        
        mV1V4IntervalEdit = (EditText)this.findViewById(R.id.et_v1v4_interval);
        mV5IntervalEdit = (EditText)this.findViewById(R.id.et_v5_interval);
        
        mWatcher = (TextView)this.findViewById(R.id.watcher);
        
        mV1CheckBox = (CheckBox)this.findViewById(R.id.checkbox_v1);
        mV4CheckBox = (CheckBox)this.findViewById(R.id.checkbox_v4);
        mV5CheckBox = (CheckBox)this.findViewById(R.id.checkbox_v5);

        mContext = this;
        mStartButton.setEnabled(true);
        mStopButton.setEnabled(false);

        boolean res = JniLoader.LoadLib();
        Log.d("SmartConnection", "Load Smart Connection Library Result " + res);
        
        loader = new JniLoader();
        
        int proV = loader.GetProtoVersion();
        int libV = loader.GetLibVersion();
        
        String version = "V" + proV + "." + libV;
        
        this.getActionBar().setTitle("SmartConnection (" + version + ")");

        mV1CheckBox.setChecked(false);
        mV4CheckBox.setChecked(false);
        mV5CheckBox.setChecked(false);
        
        mNameEdit.addTextChangedListener(new EditChangeHandler(1));
        mPswEdit.addTextChangedListener(new EditChangeHandler(2));
        mCustomInfoEdit.addTextChangedListener(new EditChangeHandler(3));
        
        WifiManager wManager = (WifiManager)this.getSystemService(Context.WIFI_SERVICE);
        WifiInfo info = wManager.getConnectionInfo();
        if (info.getSSID().isEmpty() == false) {
            String name = info.getSSID();
            String ssid = name.substring(1, name.length() - 1);
            mNameEdit.setText(ssid);
            mNameEdit.setSelection(mNameEdit.getText().length());
            Log.d("SmartConnection", "SSID " + mNameEdit.getText().toString());
        }

    }

    @Override
    protected void onStart() {
        super.onStart();

        mStartButton.setOnClickListener(new View.OnClickListener() {
            
            @Override
            public void onClick(View arg0) {
                
                int sendV1 = mV1CheckBox.isChecked() == true ? 1 : 0;
                int sendV4 = mV4CheckBox.isChecked() == true ? 1 : 0;
                int sendV5 = mV5CheckBox.isChecked() == true ? 1 : 0;
                
                float oI = 0.0f;
                float nI = 0.0f;
                if (mV1V4IntervalEdit.getText().toString().isEmpty() == false) {
                    oI = Float.parseFloat(mV1V4IntervalEdit.getText().toString());
                }
                if (mV5IntervalEdit.getText().toString().isEmpty() == false) {
                    nI = Float.parseFloat(mV5IntervalEdit.getText().toString());
                }
                
                if (sendV1 == 0 && sendV4 == 0 && sendV5 == 0) {
                    showWarningDialog(R.string.error_select_leat_1_version);
                    return;
                }
                
                int retValue = JniLoader.ERROR_CODE_OK;
                String key = mEncryptionKeyEdit.getText().toString();
                Log.d("elian_app", "Encryption Key - " + key);
                retValue = loader.InitSmartConnection(key, null, sendV1, sendV4, sendV5);
                if (retValue != JniLoader.ERROR_CODE_OK) {
                    showWarningDialog(R.string.init_failed);
                    Log.e("SmartConnection", "Init failed : " + retValue);
                    return;
                }
                loader.SetSendInterval(oI, nI);
                
                String SSID = mNameEdit.getText().toString();
                String Password = mPswEdit.getText().toString();
                String Custom = mCustomInfoEdit.getText().toString();
                Log.d("elian_app", "SSID - " + SSID + ", Password - " + Password + ", Custom - " + Custom);
                retValue = loader.StartSmartConnection(SSID, Password, Custom);
                if (retValue != JniLoader.ERROR_CODE_OK) {
                    showWarningDialog(R.string.start_failed);
                    Log.e("SmartConnection", "Start failed : " + retValue);
                    return;
                }
                
                updateUIWhileSending();
            }
        });
        mStopButton.setOnClickListener(new View.OnClickListener() {
            
            @Override
            public void onClick(View arg0) {
                
                int retValue = loader.StopSmartConnection();
                if (retValue != JniLoader.ERROR_CODE_OK) {
                    showWarningDialog(R.string.stop_failed);
                    Log.e("SmartConnection", "Stop failed : " + retValue);
                }
                
                updateUIWhileStopped();
            }
        });
    }

    @Override
    protected void onStop() {
        super.onStop();
    }
    
    private void updateUIWhileSending() {
        mStartButton.setEnabled(false);
        mStopButton.setEnabled(true);
        mProgressBar.setVisibility(View.VISIBLE);
        
        mV1CheckBox.setEnabled(false);
        mV4CheckBox.setEnabled(false);
        mV5CheckBox.setEnabled(false);
        
        mNameEdit.setEnabled(false);
        mPswEdit.setEnabled(false);
        
        this.mV1V4IntervalEdit.setEnabled(false);
        this.mV5IntervalEdit.setEnabled(false);
        mCustomInfoEdit.setEnabled(false);
    }
    
    private void updateUIWhileStopped() {
        mStartButton.setEnabled(true);
        mStopButton.setEnabled(false);
        mProgressBar.setVisibility(View.INVISIBLE);
        
        mV1CheckBox.setEnabled(true);
        mV4CheckBox.setEnabled(true);
        mV5CheckBox.setEnabled(true);
        
        mNameEdit.setEnabled(true);
        mPswEdit.setEnabled(true);
        this.mV1V4IntervalEdit.setEnabled(true);
        this.mV5IntervalEdit.setEnabled(true);
        mCustomInfoEdit.setEnabled(true);
    }
    
    private void showWarningDialog(final int stringID) {
        this.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
                builder.setTitle(R.string.warning);
                builder.setMessage(stringID);
                builder.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
                    
                    @Override
                    public void onClick(DialogInterface arg0, int arg1) {
                        arg0.dismiss();
                    }
                });
                builder.show();
            }
            
        });
    }
    /*
    private void updateError(final int type, final int error) {
        this.runOnUiThread(new Runnable() {
            public void run() {
                String str;
                switch (error) {
                case JniLoader.ERROR_CODE_NOT_INITINED:
                    str = mContext.getString(R.string.error_not_inited);
                    break;
                case JniLoader.ERROR_CODE_WRONG_TYPE:
                    str = mContext.getString(R.string.error_wrong_type);
                    break;
                case JniLoader.ERROR_CODE_WRONG_PARAMETER:
                    str = mContext.getString(R.string.error_wrong_parameter);
                    break;
                case JniLoader.ERROR_CODE_CRYPTED_FAILED:
                    str = mContext.getString(R.string.error_crypt_failed);
                    break;
                case JniLoader.ERROR_CODE_NOT_ENOUGH_MEMORY:
                    str = mContext.getString(R.string.error_not_enough_memory);
                    break;
                case JniLoader.ERROR_CODE_EXCEED_MAX_LENGTH:
                    str = mContext.getString(R.string.error_exceed_max_length);
                    break;
                case JniLoader.ERROR_CODE_ALREADY_STARTED:
                    str = mContext.getString(R.string.error_already_started);
                    break;
                case JniLoader.ERROR_CODE_ALREADY_STOPED:
                    str = mContext.getString(R.string.error_already_stopped);
                    break;
                case JniLoader.ERROR_CODE_FAILED_TO_START:
                    str = mContext.getString(R.string.error_failed_to_start);
                    break;
                    default:
                        str = mContext.getString(R.string.error_unkonwn_error);
                        break;
                }
                
            }
        });
    }*/

    private class EditChangeHandler implements TextWatcher {

        private int which;
        
        public EditChangeHandler(int which) {
            this.which = which;
        }
        
        @Override
        public void afterTextChanged(Editable arg0) {
            // TODO Auto-generated method stub
            
        }

        @Override
        public void beforeTextChanged(CharSequence arg0, int arg1, int arg2,
                int arg3) {
            // TODO Auto-generated method stub
            
        }

        @Override
        public void onTextChanged(CharSequence arg0, int arg1, int arg2,
                int arg3) {
            // TODO Auto-generated method stub
            String str = arg0.toString();
            boolean hid = false;
            
            switch(which) {
            case 1:
                if (str.getBytes().length > 32) {
                    mWatcher.setText("SSID Exceed Max Length (32)");
                } else {
                    hid = true;
                }
                break;
            case 2:
                if (str.getBytes().length > 64) {
                    mWatcher.setText("Password Exceed Max Length (64)");
                } else {
                    hid = true;
                }
                break;
            case 3:
                if (str.getBytes().length > 640) {
                    mWatcher.setText("CustomInfo Exceed Max Length (640)");
                } else {
                    hid = true;
                }
                break;
            }
            if (hid == true) {
                mWatcher.setVisibility(View.GONE);
            } else {
                mWatcher.setVisibility(View.VISIBLE);
            }
        }
        
    }
    
}
