package com.remoteaccess.app;

import org.qtproject.qt.android.bindings.QtActivity;

public class MainActivity extends QtActivity {
    static {
        System.loadLibrary("RemoteAccessMobile_arm64-v8a");
    }
}
