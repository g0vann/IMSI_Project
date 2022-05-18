package analog.com.IoTNode.sensors;

import analog.com.IoTNode.Utilities;

public class AccelerometerSensor extends MainSensor {
    public AccelerometerSensor(int sensorID, double timeStamp, byte[] data) {
        super(sensorID, "Accelerometer", 20, timeStamp, data, SensorConfiguration.LEGEND_ACCEL_CFG);
    }

    public double[] parseData(byte[] sensorData) {
        int[] data = Utilities.signedToUnsigned(sensorData);
        return new double[]{(double) ((short) (((short) data[6]) | (data[7] << 8))), (double) ((short) (((short) data[8]) | (data[9] << 8))), (double) ((short) (((short) data[10]) | (data[11] << 8)))};
    }
}
