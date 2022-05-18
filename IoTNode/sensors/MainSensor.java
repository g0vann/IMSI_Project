package analog.com.IoTNode.sensors;

import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;
import java.util.ArrayList;

public abstract class MainSensor {
    private long mLastDataReceived = 0;
    private int mNumDataPoints;
    String mPrintString = "";
    private int mSensorId;
    private String mSensorName = "";
    private ArrayList<LineGraphSeries<DataPoint>> mSeries = new ArrayList<>();

    public abstract double[] parseData(byte[] bArr);

    MainSensor(int sensorID, String name) {
        this.mSensorName = name;
        this.mSensorId = sensorID;
    }

    MainSensor(int sensorID, String name, int numDataPoints, double timeStamp, byte[] data, String[] legend) {
        this.mSensorName = name;
        this.mSensorId = sensorID;
        this.mNumDataPoints = numDataPoints;
        createSeries(timeStamp, data, legend);
    }

    public String getSensorName() {
        return this.mSensorName;
    }

    public String getPrintString() {
        return this.mPrintString;
    }

    public ArrayList<LineGraphSeries<DataPoint>> getSeries() {
        return this.mSeries;
    }

    public int getId() {
        return this.mSensorId;
    }

    public String getTimeMessage(int timeout) {
        if (System.currentTimeMillis() - this.mLastDataReceived <= ((long) (timeout * 1000))) {
            return "";
        }
        long lastTime = System.currentTimeMillis() - this.mLastDataReceived;
        long second = (lastTime / 1000) % 60;
        long minute = (lastTime / 60000) % 60;
        long hour = (lastTime / 3600000) % 24;
        String secondS = second + "";
        if (second < 10) {
            secondS = "0" + secondS;
        }
        if (hour > 0) {
            if (hour < 2) {
                return "Last packet received " + hour + ":" + minute + ":" + secondS + " hour ago";
            }
            return "Last packet received " + hour + ":" + minute + ":" + secondS + " hours ago";
        } else if (minute <= 0) {
            return "Last packet received " + secondS + " seconds ago";
        } else {
            if (minute < 2) {
                return "Last packet received " + minute + ":" + secondS + " minute ago";
            }
            return "Last packet received " + minute + ":" + secondS + " minutes ago";
        }
    }

    public void updateSeries(double timeStamp, byte[] sensorData) {
        double[] ParsedData = parseData(sensorData);
        for (int j = 0; j < this.mSeries.size(); j++) {
            if (this.mSeries.get(j).getHighestValueX() < timeStamp) {
                this.mSeries.get(j).appendData(new DataPoint(timeStamp, ParsedData[j]), false, this.mNumDataPoints);
            }
        }
        this.mLastDataReceived = System.currentTimeMillis();
    }

    public void createSeries(double timeStamp, byte[] data, String[] legend) {
        double[] ParsedData = parseData(data);
        this.mLastDataReceived = System.currentTimeMillis();
        for (int j = 0; j < legend.length; j++) {
            this.mSeries.add(j, new LineGraphSeries(new DataPoint[]{new DataPoint(timeStamp, ParsedData[j])}));
            if (legend[j] != null) {
                this.mSeries.get(j).setTitle(legend[j]);
            }
        }
    }
}
