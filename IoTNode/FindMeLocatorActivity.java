package analog.com.IoTNode;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.p003v7.app.AppCompatActivity;
import android.view.View;

public class FindMeLocatorActivity extends AppCompatActivity {
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals("ADI_BLE_DISCONNECTED_EVENT")) {
                FindMeLocatorActivity.this.finish();
            }
        }
    };

    /* access modifiers changed from: protected */
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView((int) C0009R.layout.activity_findme_locator);
        IntentFilter filter = new IntentFilter();
        filter.addAction("ADI_BLE_DISCONNECTED_EVENT");
        registerReceiver(this.mReceiver, filter);
    }

    /* access modifiers changed from: protected */
    public void onDestroy() {
        super.onDestroy();
        sendBroadcast(new Intent("ADI_BLE_GATT_DISCONNECT"));
        unregisterReceiver(this.mReceiver);
    }

    public void onStop() {
        super.onStop();
    }

    public void onNoAlert(View v) {
        sendAlert(SharedDefines.NO_ALERT);
    }

    public void onMediumAlert(View v) {
        sendAlert(SharedDefines.MEDIUM_ALERT);
    }

    public void onHighAlert(View v) {
        sendAlert(SharedDefines.HIGH_ALERT);
    }

    public void onDisconnect(View v) {
        finish();
    }

    private void sendAlert(byte[] level) {
        Intent intent = new Intent("ADI_BLE_ALERT_LEVEL");
        intent.putExtra("LEVEL", level);
        sendBroadcast(intent);
    }
}
