package analog.com.IoTNode.sensors;

import analog.com.IoTNode.Utilities;

public class GenericSensor extends MainSensor {
    public GenericSensor(int sensorID, byte[] data) {
        super(sensorID, "Generic");
        parseData(data);
    }

    public double[] parseData(byte[] sensorData) {
        int[] data = Utilities.signedToUnsigned(sensorData);
        int mLength = data[6];
        if (mLength > 13) {
            mLength = 13;
        }
        char[] printStringArray = new char[mLength];
        for (int i = 0; i < mLength; i++) {
            printStringArray[i] = (char) data[i + 7];
        }
        this.mPrintString = new String(printStringArray);
        return new double[0];
    }
}
