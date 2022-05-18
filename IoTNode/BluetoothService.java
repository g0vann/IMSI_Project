package analog.com.IoTNode;

import android.annotation.TargetApi;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.support.p000v4.media.session.PlaybackStateCompat;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.UUID;

public class BluetoothService extends Service {
    private BluetoothLeScanner mBLEScanner = null;
    private BluetoothAdapter mBluetoothAdapter = null;
    /* access modifiers changed from: private */
    public BluetoothGatt mBluetoothGatt = null;
    private final BluetoothGattCallback mBluetoothGattCallback = new GattCallback();
    /* access modifiers changed from: private */
    public boolean mConnecting = false;
    /* access modifiers changed from: private */
    public final ArrayList<BluetoothDevice> mDiscoveredDevices = new ArrayList<>();
    /* access modifiers changed from: private */
    public BluetoothGattCharacteristic mImmediateAlertLevelCharacteristic = null;
    private BluetoothGattCharacteristic mLinkLossAlertLevelCharacteristic = null;
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals("android.bluetooth.device.action.FOUND") && Build.VERSION.SDK_INT < 23) {
                BluetoothDevice device = (BluetoothDevice) intent.getParcelableExtra("android.bluetooth.device.extra.DEVICE");
                Iterator it = BluetoothService.this.mDiscoveredDevices.iterator();
                while (it.hasNext()) {
                    if (((BluetoothDevice) it.next()).getAddress().equals(device.getAddress())) {
                        return;
                    }
                }
                Intent nIntent = new Intent("ADI_BLE_ADD_DEVICE");
                nIntent.putExtra("DEVICE_INFO", new String[]{device.getName(), device.getAddress(), intent.getShortExtra("android.bluetooth.device.extra.RSSI", Short.MIN_VALUE) + " dBm"});
                BluetoothService.this.sendBroadcast(nIntent);
                BluetoothService.this.mDiscoveredDevices.add(device);
            }
            if (action.equals("ADI_BLE_STOP_DISCOVERY")) {
                BluetoothService.this.stopDiscovery();
            }
            if (action.equals("ADI_BLE_START_DISCOVERY")) {
                BluetoothService.this.startDiscovery();
            }
            if (action.equals("ADI_BLE_ALERT_LEVEL") && BluetoothService.this.mImmediateAlertLevelCharacteristic != null) {
                BluetoothService.this.mImmediateAlertLevelCharacteristic.setValue(intent.getByteArrayExtra("LEVEL"));
                BluetoothService.this.writeCharacteristic(BluetoothService.this.mImmediateAlertLevelCharacteristic);
            }
            if (action.equals("ADI_BLE_CONNECT_TO_DEVICE")) {
                if (!BluetoothService.this.mConnecting) {
                    boolean unused = BluetoothService.this.mConnecting = true;
                    BluetoothService.this.enumerateBluetoothServices(intent.getIntExtra("DEVICE_POSITION", 0));
                } else {
                    return;
                }
            }
            if (action.equals("ADI_BLE_GATT_DISCONNECT")) {
                BluetoothService.this.gattDisconnect();
            }
            if (action.equals("ADI_BLE_GET_RSSI") && BluetoothService.this.mBluetoothGatt != null) {
                BluetoothService.this.mBluetoothGatt.readRemoteRssi();
            }
        }
    };
    private ScanCallback mScanCallback = null;

    public void onCreate() {
        System.out.println("____CREATE");
        IntentFilter filter = new IntentFilter();
        for (String str : new String[]{"android.bluetooth.device.action.FOUND", "ADI_BLE_GATT_DISCONNECT", "ADI_BLE_ALERT_LEVEL", "ADI_BLE_GET_RSSI", "ADI_BLE_STOP_DISCOVERY", "ADI_BLE_START_DISCOVERY", "ADI_BLE_CONNECT_TO_DEVICE"}) {
            filter.addAction(str);
        }
        registerReceiver(this.mReceiver, filter);
        initializeBluetooth();
        initializeBLE();
    }

    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(this.mReceiver);
    }

    @Nullable
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void initializeBluetooth() {
        this.mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (this.mBluetoothAdapter == null) {
            sendBroadcast(new Intent("ADI_BLE_NO_BLUETOOTH"));
        } else if (!this.mBluetoothAdapter.isEnabled()) {
            sendBroadcast(new Intent("ADI_BLE_ENABLE_BLUETOOTH"));
        }
    }

    @TargetApi(23)
    private void initializeBLE() {
        if (Build.VERSION.SDK_INT >= 23) {
            if (this.mBluetoothAdapter.isEnabled()) {
                this.mBLEScanner = this.mBluetoothAdapter.getBluetoothLeScanner();
            }
            this.mScanCallback = new ScanCallback() {
                public void onScanResult(int callbackType, ScanResult result) {
                    BluetoothDevice device = result.getDevice();
                    Iterator it = BluetoothService.this.mDiscoveredDevices.iterator();
                    while (it.hasNext()) {
                        if (((BluetoothDevice) it.next()).getAddress().equals(device.getAddress())) {
                            return;
                        }
                    }
                    Intent intent = new Intent("ADI_BLE_ADD_DEVICE");
                    intent.putExtra("DEVICE_INFO", new String[]{device.getName(), device.getAddress(), result.getRssi() + " dBm"});
                    BluetoothService.this.sendBroadcast(intent);
                    BluetoothService.this.mDiscoveredDevices.add(device);
                }
            };
        }
    }

    /* access modifiers changed from: private */
    public void enumerateBluetoothServices(int i) {
        this.mBluetoothGatt = this.mBluetoothAdapter.getRemoteDevice(this.mDiscoveredDevices.get(i).getAddress()).connectGatt(this, false, this.mBluetoothGattCallback);
        if (this.mBluetoothGatt != null) {
            this.mBluetoothGatt.discoverServices();
        }
        stopDiscovery();
        this.mBluetoothGatt.connect();
        this.mConnecting = false;
    }

    /* access modifiers changed from: private */
    public int getBluetoothProfile() {
        if (this.mBluetoothGatt == null) {
            return 0;
        }
        List<BluetoothGattService> gattServices = this.mBluetoothGatt.getServices();
        for (BluetoothGattService service : gattServices) {
            long shortUUID = getShortUUID(service.getUuid());
            if (shortUUID == 6146 && gattServices.size() == 4) {
                return 1;
            }
            if (shortUUID == 21248 && gattServices.size() == 4) {
                return 3;
            }
            if (shortUUID == 6147 && gattServices.size() == 5) {
                return 2;
            }
        }
        gattDisconnect();
        return 0;
    }

    /* access modifiers changed from: private */
    public void startDiscovery() {
        this.mDiscoveredDevices.clear();
        if (Build.VERSION.SDK_INT >= 23) {
            if (this.mBLEScanner == null || !this.mBluetoothAdapter.isEnabled()) {
                this.mBLEScanner = null;
                initializeBluetooth();
                return;
            }
            this.mBLEScanner.startScan(this.mScanCallback);
        } else if (this.mBluetoothAdapter.isEnabled()) {
            this.mBluetoothAdapter.startDiscovery();
        } else {
            initializeBluetooth();
        }
    }

    /* access modifiers changed from: private */
    public void stopDiscovery() {
        if (Build.VERSION.SDK_INT >= 23) {
            if (this.mBLEScanner != null) {
                this.mBLEScanner.stopScan(this.mScanCallback);
            }
        } else if (this.mBluetoothAdapter != null) {
            this.mBluetoothAdapter.cancelDiscovery();
        }
    }

    /* access modifiers changed from: private */
    public void gattDisconnect() {
        if (this.mBluetoothGatt != null) {
            this.mBluetoothGatt.disconnect();
            this.mBluetoothGatt.close();
            this.mBluetoothGatt = null;
            this.mConnecting = false;
        }
    }

    public void setupCharacteristics() {
        if (this.mBluetoothGatt != null) {
            for (BluetoothGattService service : this.mBluetoothGatt.getServices()) {
                long shortUUID = getShortUUID(service.getUuid());
                if (shortUUID == 21248) {
                    BluetoothGattCharacteristic deReadChannelCharacteristic = service.getCharacteristic(SharedDefines.DATAEXCHANGE_READ_CHANNEL_UUID);
                    BluetoothGattDescriptor descriptor = deReadChannelCharacteristic.getDescriptor(SharedDefines.DATAEXCHANGE_READ_WRITE_DESCRIPTOR_UUID);
                    descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                    this.mBluetoothGatt.writeDescriptor(descriptor);
                    this.mBluetoothGatt.setCharacteristicNotification(deReadChannelCharacteristic, true);
                }
                if (shortUUID == 6146) {
                    this.mImmediateAlertLevelCharacteristic = service.getCharacteristic(SharedDefines.IMMEDIATE_ALERT_ALERT_LEVEL_UUID);
                }
                if (shortUUID == 6147) {
                    this.mLinkLossAlertLevelCharacteristic = service.getCharacteristic(SharedDefines.IMMEDIATE_ALERT_ALERT_LEVEL_UUID);
                }
            }
        }
    }

    /* access modifiers changed from: private */
    public void writeCharacteristic(BluetoothGattCharacteristic characteristic) {
        this.mBluetoothGatt.writeCharacteristic(characteristic);
    }

    private static long getShortUUID(UUID uuid) {
        return (uuid.getMostSignificantBits() - PlaybackStateCompat.ACTION_SKIP_TO_QUEUE_ITEM) >> 32;
    }

    private class GattCallback extends BluetoothGattCallback {
        private GattCallback() {
        }

        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            if (newState == 2) {
                System.out.println("____CONNECTED");
                BluetoothService.this.mBluetoothGatt.discoverServices();
            } else if (newState == 0) {
                System.out.println("DEVICE STATE_DISCONNECTED");
                BluetoothService.this.sendBroadcast(new Intent("ADI_BLE_DISCONNECTED_EVENT"));
            }
        }

        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == 0) {
                int selectedProfile = BluetoothService.this.getBluetoothProfile();
                BluetoothService.this.setupCharacteristics();
                Intent intent = new Intent("ADI_BLE_SERVICE_DISCOVERY_COMPLETE");
                intent.putExtra("SELECTED_PROFILE", selectedProfile);
                BluetoothService.this.sendBroadcast(intent);
            }
        }

        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            Intent intent = new Intent("ADI_BLE_CHARACTERISTIC_CHANGED");
            intent.putExtra("NEW_VALUE", characteristic.getValue());
            BluetoothService.this.sendBroadcast(intent);
        }

        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            Intent intent = new Intent("ADI_BLE_GET_PATH_LOSS");
            intent.putExtra("RSSI", rssi);
            BluetoothService.this.sendBroadcast(intent);
        }
    }
}
