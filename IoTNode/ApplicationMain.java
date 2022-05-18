package analog.com.IoTNode;

import android.annotation.TargetApi;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.support.p003v7.app.AlertDialog;
import android.support.p003v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.Toast;

public class ApplicationMain extends AppCompatActivity {
    static final /* synthetic */ boolean $assertionsDisabled = (!ApplicationMain.class.desiredAssertionStatus());
    private final Handler mDiscoveryTimeoutHandler = new Handler();
    private final DiscoveryTimeoutRunnable mDiscoveryTimeoutRunnable = new DiscoveryTimeoutRunnable();
    /* access modifiers changed from: private */
    public DeviceDisplayListAdapter mListAdapter = null;
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals("ADI_BLE_SERVICE_DISCOVERY_COMPLETE")) {
                ApplicationMain.this.toggleServiceScanning(false);
                ApplicationMain.this.toggleScanButtons(false);
                int selectedProfile = intent.getIntExtra("SELECTED_PROFILE", 0);
                if (selectedProfile == 1) {
                    ApplicationMain.this.startActivity(new Intent(ApplicationMain.this.getApplicationContext(), FindMeLocatorActivity.class));
                } else if (selectedProfile == 3) {
                    ApplicationMain.this.startActivity(new Intent(ApplicationMain.this.getApplicationContext(), SensorDisplayActivity.class));
                } else if (selectedProfile == 2) {
                    ApplicationMain.this.startActivity(new Intent(ApplicationMain.this.getApplicationContext(), ProximityMonitorActivity.class));
                } else {
                    AlertDialog alertDialog = new AlertDialog.Builder(ApplicationMain.this).create();
                    alertDialog.setTitle("Alert");
                    alertDialog.setMessage("No Application Exists for the enumerated Services");
                    alertDialog.setButton(-3, (CharSequence) "OK", (DialogInterface.OnClickListener) new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                        }
                    });
                    alertDialog.show();
                }
            }
            if (action.equals("ADI_BLE_ENABLE_BLUETOOTH")) {
                ApplicationMain.this.startActivityForResult(new Intent("android.bluetooth.adapter.action.REQUEST_ENABLE"), 1);
            }
            if (action.equals("ADI_BLE_NO_BLUETOOTH")) {
                AlertDialog alertDialog2 = new AlertDialog.Builder(ApplicationMain.this).create();
                alertDialog2.setTitle("Alert");
                alertDialog2.setMessage("This device doesn't support Bluetooth");
                alertDialog2.setButton(-3, (CharSequence) "OK", (DialogInterface.OnClickListener) new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
                alertDialog2.show();
            }
            if (action.equals("ADI_BLE_ADD_DEVICE")) {
                ApplicationMain.this.mListAdapter.addDevice(intent.getStringArrayExtra("DEVICE_INFO"));
            }
        }
    };
    private final Handler mServiceScanTimeoutHandler = new Handler();
    private final ServiceScanTimeoutRunnable mServiceScanTimeoutRunnable = new ServiceScanTimeoutRunnable();
    private ProgressDialog mServiceScanningDialog = null;

    /* access modifiers changed from: protected */
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView((int) C0009R.layout.activity_application_main);
        this.mServiceScanningDialog = new ProgressDialog(this);
        this.mServiceScanningDialog.setTitle("Discover Services");
        this.mServiceScanningDialog.setMessage("Scanning Remote Device Services..");
        checkLocationPermission();
        initBroadcastReceiver();
        startService(new Intent(getApplicationContext(), BluetoothService.class));
        this.mListAdapter = new DeviceDisplayListAdapter(this);
        ListView mListView = (ListView) findViewById(C0009R.C0011id.BLEDevList);
        if ($assertionsDisabled || mListView != null) {
            mListView.setAdapter(this.mListAdapter);
            mListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                    ApplicationMain.this.toggleServiceScanning(true);
                    Intent intent = new Intent("ADI_BLE_CONNECT_TO_DEVICE");
                    intent.putExtra("DEVICE_POSITION", i);
                    ApplicationMain.this.sendBroadcast(intent);
                }
            });
            return;
        }
        throw new AssertionError();
    }

    /* access modifiers changed from: protected */
    public void onPause() {
        super.onPause();
        toggleScanButtons(false);
        sendBroadcast(new Intent("ADI_BLE_STOP_DISCOVERY"));
    }

    /* access modifiers changed from: protected */
    public void onStop() {
        super.onStop();
        toggleScanButtons(false);
        sendBroadcast(new Intent("ADI_BLE_STOP_DISCOVERY"));
    }

    public void onDestroy() {
        super.onDestroy();
        stopService(new Intent(this, BluetoothService.class));
        unregisterReceiver(this.mReceiver);
    }

    public void onClickScan(View v) {
        this.mListAdapter.clearList();
        toggleScanButtons(true);
        sendBroadcast(new Intent("ADI_BLE_START_DISCOVERY"));
        if (this.mDiscoveryTimeoutHandler != null) {
            this.mDiscoveryTimeoutHandler.postDelayed(this.mDiscoveryTimeoutRunnable, 12000);
        }
    }

    public void onClickScanStop(View v) {
        if (this.mDiscoveryTimeoutHandler != null) {
            this.mDiscoveryTimeoutHandler.removeCallbacks(this.mDiscoveryTimeoutRunnable);
        }
        toggleScanButtons(false);
        sendBroadcast(new Intent("ADI_BLE_STOP_DISCOVERY"));
    }

    @TargetApi(23)
    private void checkLocationPermission() {
        if (Build.VERSION.SDK_INT >= 23 && checkSelfPermission("android.permission.ACCESS_COARSE_LOCATION") != 0) {
            requestPermissions(new String[]{"android.permission.ACCESS_COARSE_LOCATION"}, 1);
        }
    }

    private void initBroadcastReceiver() {
        IntentFilter filter = new IntentFilter();
        for (String str : new String[]{"ADI_BLE_SERVICE_DISCOVERY_COMPLETE", "ADI_BLE_ENABLE_BLUETOOTH", "ADI_BLE_NO_BLUETOOTH", "ADI_BLE_ADD_DEVICE"}) {
            filter.addAction(str);
        }
        registerReceiver(this.mReceiver, filter);
    }

    /* access modifiers changed from: private */
    public void toggleServiceScanning(boolean toggle) {
        if (toggle) {
            this.mServiceScanningDialog.show();
            this.mServiceScanningDialog.setCancelable(true);
            if (this.mServiceScanTimeoutHandler != null) {
                this.mServiceScanTimeoutHandler.postDelayed(this.mServiceScanTimeoutRunnable, 7000);
                return;
            }
            return;
        }
        if (this.mServiceScanTimeoutHandler != null) {
            this.mServiceScanTimeoutHandler.removeCallbacks(this.mServiceScanTimeoutRunnable);
        }
        this.mServiceScanningDialog.dismiss();
    }

    /* access modifiers changed from: protected */
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == 0) {
            Toast.makeText(getApplicationContext(), "No Bluetooth detected. Please enable Bluetooth in your device settings.", 1).show();
        }
    }

    /* access modifiers changed from: private */
    public void toggleScanButtons(boolean scanning) {
        ProgressBar progressBar = (ProgressBar) findViewById(C0009R.C0011id.progressbar);
        if (!$assertionsDisabled && progressBar == null) {
            throw new AssertionError();
        } else if (scanning) {
            toggleButton(C0009R.C0011id.scan, false);
            toggleButton(C0009R.C0011id.stopScan, true);
            progressBar.setVisibility(0);
        } else {
            toggleButton(C0009R.C0011id.scan, true);
            toggleButton(C0009R.C0011id.stopScan, false);
            progressBar.setVisibility(4);
        }
    }

    private void toggleButton(int id, boolean enable) {
        Button button = (Button) findViewById(id);
        if (button != null) {
            button.setEnabled(enable);
        }
    }

    private class ServiceScanTimeoutRunnable implements Runnable {
        private ServiceScanTimeoutRunnable() {
        }

        public void run() {
            ApplicationMain.this.toggleServiceScanning(false);
            Toast.makeText(ApplicationMain.this.getApplicationContext(), "Couldn't establish connection, please try again", 1).show();
            ApplicationMain.this.sendBroadcast(new Intent("ADI_BLE_GATT_DISCONNECT"));
        }
    }

    private class DiscoveryTimeoutRunnable implements Runnable {
        private DiscoveryTimeoutRunnable() {
        }

        public void run() {
            ApplicationMain.this.toggleScanButtons(false);
            ApplicationMain.this.sendBroadcast(new Intent("ADI_BLE_STOP_DISCOVERY"));
        }
    }
}
