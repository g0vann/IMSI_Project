package analog.com.IoTNode.sensors;

import java.nio.ByteBuffer;

public class C0Sensor extends MainSensor {
    public C0Sensor(int sensorID, double timeStamp, byte[] data) {
        super(sensorID, "C0", 20, timeStamp, data, SensorConfiguration.LEGEND_C0_CFG);
    }

    public double[] parseData(byte[] sensorData) {
        return new double[]{(double) ByteBuffer.wrap(new byte[]{sensorData[9], sensorData[8], sensorData[7], sensorData[6]}).getFloat()};
    }
}
