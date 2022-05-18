package analog.com.IoTNode;

import android.support.p000v4.internal.view.SupportMenu;
import android.support.p000v4.view.InputDeviceCompat;
import java.util.UUID;

abstract class SharedDefines {
    static final int ACCELEROMETER_2G_SENSOR = 1;
    static final int ACCELEROMETER_4G_SENSOR = 11;
    static final int ACCELEROMETER_8G_SENSOR = 12;
    static final long BT_UUID_UPPER_BITS = 4096;
    static final int CO_SENSOR = 2;
    static final int DATAEXCHANGE_PROFILE = 3;
    static final UUID DATAEXCHANGE_READ_CHANNEL_UUID = UUID.fromString("00005303-0000-0041-4c50-574953450000");
    static final UUID DATAEXCHANGE_READ_WRITE_DESCRIPTOR_UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    static final int DATAEXCHANGE_SERVICE_SIZE = 4;
    static final int DATAEXCHANGE_SHORT_UUID = 21248;
    static final int DATA_PACKET = 1;
    static final int DATA_POINT_RADIUS_CFG = 5;
    static final int DATA_RECEIVED_TIMEOUT_CFG = 15;
    static final int DISCOVERY_TIMEOUT_CFG = 12;
    static final int ESTABLISH_CONN_TIMEOUT_CFG = 7;
    static final int FINDME_PROFILE = 1;
    static final int FINDME_SERVICE_SIZE = 4;
    static final int FINDME_SHORT_UUID = 6146;
    static final int GENERIC_SENSOR = 0;
    static final int[] GRAPH_SERIES_COLORS_CFG = {SupportMenu.CATEGORY_MASK, -16711936, -16776961, -65281, InputDeviceCompat.SOURCE_ANY, -16711681};
    static final byte[] HIGH_ALERT = {2};
    static final UUID IMMEDIATE_ALERT_ALERT_LEVEL_UUID = UUID.fromString("00002a06-0000-1000-8000-00805f9b34fb");
    static final byte[] MEDIUM_ALERT = {1};
    static final byte[] NO_ALERT = {0};
    static final int PATH_LOSS_SLEEP_TIME_CFG = 500;
    static final int PRINT_STRING_SENSOR = 5;
    static final int PROXIMITY_PROFILE = 2;
    static final int PROXIMITY_SERVICE_SIZE = 5;
    static final int PROXIMITY_SHORT_UUID = 6147;
    static final int REGISTRATION_PACKET = 0;
    static final int TEMPERATURE_SENSOR = 3;
    static final int VISUAL_LIGHT_SENSOR = 4;
    static final int X_AXIS_OFFSET_CFG = 2;

    SharedDefines() {
    }
}
