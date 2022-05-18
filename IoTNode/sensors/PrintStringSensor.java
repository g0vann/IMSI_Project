package analog.com.IoTNode.sensors;

import analog.com.IoTNode.Utilities;

public class PrintStringSensor extends MainSensor {
    public PrintStringSensor(int sensorID, byte[] data) {
        super(sensorID, "Print String");
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
