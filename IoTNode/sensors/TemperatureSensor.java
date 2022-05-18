package analog.com.IoTNode.sensors;

import java.nio.ByteBuffer;

public class TemperatureSensor extends MainSensor {
    public TemperatureSensor(int sensorID, double timeStamp, byte[] data) {
        super(sensorID, "Temperature", 5, timeStamp, data, SensorConfiguration.LEGEND_TEMP_CFG);
    }

    public double[] parseData(byte[] sensorData) {
        double[] retTemp = new double[2];
        retTemp[0] = (double) ByteBuffer.wrap(new byte[]{sensorData[9], sensorData[8], sensorData[7], sensorData[6]}).getFloat();
        retTemp[1] = ((retTemp[0] * 9.0d) / 5.0d) + 32.0d;
        return retTemp;
    }
}
