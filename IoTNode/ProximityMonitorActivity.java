package analog.com.IoTNode;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.support.p003v7.app.AppCompatActivity;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

public class ProximityMonitorActivity extends AppCompatActivity {
    private final PathLossThread mPathLossThread = new PathLossThread();
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals("ADI_BLE_GET_PATH_LOSS")) {
                int distance = 0 - intent.getIntExtra("RSSI", 0);
                ((TextView) ProximityMonitorActivity.this.findViewById(C0009R.C0011id.distance)).setText(" " + distance);
                int progress = ((SeekBar) ProximityMonitorActivity.this.findViewById(C0009R.C0011id.slideBar)).getProgress();
                ((TextView) ProximityMonitorActivity.this.findViewById(C0009R.C0011id.seekBarValue)).setText(String.valueOf(progress));
                if (distance >= progress) {
                    ProximityMonitorActivity.this.sendAlert(SharedDefines.MEDIUM_ALERT);
                }
            }
            if (action.equals("ADI_BLE_DISCONNECTED_EVENT")) {
                ProximityMonitorActivity.this.finish();
            }
        }
    };
    private final int mTxPowerLevel = 0;

    /* access modifiers changed from: protected */
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView((int) C0009R.layout.activity_proximity);
        new Thread(this.mPathLossThread).start();
        IntentFilter filter = new IntentFilter();
        filter.addAction("ADI_BLE_GET_PATH_LOSS");
        filter.addAction("ADI_BLE_DISCONNECTED_EVENT");
        registerReceiver(this.mReceiver, filter);
    }

    /* access modifiers changed from: protected */
    public void onDestroy() {
        super.onDestroy();
        sendBroadcast(new Intent("ADI_BLE_GATT_DISCONNECT"));
        unregisterReceiver(this.mReceiver);
        this.mPathLossThread.stop();
    }

    public void onDisconnect(View v) {
        finish();
    }

    /* access modifiers changed from: private */
    public void sendAlert(byte[] level) {
        Intent intent = new Intent("ADI_BLE_ALERT_LEVEL");
        intent.putExtra("LEVEL", level);
        sendBroadcast(intent);
    }

    private class PathLossThread implements Runnable {
        private volatile boolean exit;

        private PathLossThread() {
            this.exit = false;
        }

        public void run() {
            while (!this.exit) {
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                ProximityMonitorActivity.this.sendBroadcast(new Intent("ADI_BLE_GET_RSSI"));
            }
        }

        /* access modifiers changed from: package-private */
        public void stop() {
            this.exit = true;
        }
    }
}
