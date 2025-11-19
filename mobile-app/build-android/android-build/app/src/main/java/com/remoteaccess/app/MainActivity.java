package com.remoteaccess.app;
import android.app.Activity;
import android.os.Bundle;
public class MainActivity extends Activity {
    static { System.loadLibrary("remote-access-mobile"); }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
}
