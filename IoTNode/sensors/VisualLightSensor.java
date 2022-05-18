package analog.com.IoTNode.sensors;

import java.nio.ByteBuffer;

public class VisualLightSensor extends MainSensor {
    public VisualLightSensor(int sensorID, double timeStamp, byte[] data) {
        super(sensorID, "Visual Light", 20, timeStamp, data, SensorConfiguration.LEGEND_LIGHT_CFG);
    }

    public double[] parseData(byte[] sensorData) {
        byte[] temp = {sensorData[9], sensorData[8], sensorData[7], sensorData[6]};
        temp[0] = sensorData[13];
        temp[1] = sensorData[12];
        temp[2] = sensorData[11];
        temp[3] = sensorData[10];
        temp[0] = sensorData[17];
        temp[1] = sensorData[16];
        temp[2] = sensorData[15];
        temp[3] = sensorData[14];
        return new double[]{(double) ByteBuffer.wrap(temp).getFloat(), (double) ByteBuffer.wrap(temp).getFloat(), (double) ByteBuffer.wrap(temp).getFloat()};
    }
}
