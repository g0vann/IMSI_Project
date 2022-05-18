package analog.com.IoTNode;

import analog.com.IoTNode.sensors.AccelerometerSensor;
import analog.com.IoTNode.sensors.C0Sensor;
import analog.com.IoTNode.sensors.GenericSensor;
import analog.com.IoTNode.sensors.MainSensor;
import analog.com.IoTNode.sensors.TemperatureSensor;
import analog.com.IoTNode.sensors.VisualLightSensor;
import android.content.Context;
import android.support.p000v4.media.TransportMediator;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;
import com.jjoe64.graphview.GraphView;
import java.util.ArrayList;
import java.util.Iterator;

class SensorDisplayListAdapter extends BaseAdapter {
    private LayoutInflater mLayoutInflater = null;
    private final ArrayList<MainSensor> mSensorList = new ArrayList<>();

    SensorDisplayListAdapter(Context mContext) {
        this.mLayoutInflater = LayoutInflater.from(mContext);
    }

    public int getCount() {
        return this.mSensorList.size();
    }

    public MainSensor getItem(int position) {
        return this.mSensorList.get(position);
    }

    public long getItemId(int position) {
        return (long) this.mSensorList.get(position).getId();
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            ViewHolder holder = new ViewHolder();
            convertView = this.mLayoutInflater.inflate(C0009R.layout.sensor_display_listview, parent, false);
            holder.sensorName = (TextView) convertView.findViewById(C0009R.C0011id.sensorName);
            holder.sensorId = (TextView) convertView.findViewById(C0009R.C0011id.sensorId);
            holder.printString = (TextView) convertView.findViewById(C0009R.C0011id.printString);
            holder.timeMessage = (TextView) convertView.findViewById(C0009R.C0011id.timeMessage);
            holder.graphView = (GraphView) convertView.findViewById(C0009R.C0011id.sensorGraphView);
            holder.sensorName.setText(this.mSensorList.get(position).getSensorName());
            holder.sensorId.setText("ID # " + this.mSensorList.get(position).getId());
            holder.sensorId.setTag(Integer.valueOf(this.mSensorList.get(position).getId()));
            holder.graphView.removeAllSeries();
            if (!(this.mSensorList.get(position) instanceof GenericSensor)) {
                Utilities.setGraphVisuals(holder.graphView);
                Utilities.addGraphSeries(holder.graphView, this.mSensorList.get(position).getSeries());
            } else {
                holder.printString.setText(this.mSensorList.get(position).getPrintString());
                Utilities.hideGraphSeries(holder.graphView);
            }
            convertView.setTag(holder);
        } else {
            ViewHolder holder2 = (ViewHolder) convertView.getTag();
            if (this.mSensorList.get(position) instanceof GenericSensor) {
                holder2.printString.setText(this.mSensorList.get(position).getPrintString());
                Utilities.hideGraphSeries(holder2.graphView);
            } else if (((Integer) holder2.sensorId.getTag()).intValue() != this.mSensorList.get(position).getId()) {
                holder2.sensorName.setText(this.mSensorList.get(position).getSensorName());
                holder2.sensorId.setText("ID # " + this.mSensorList.get(position).getId());
                holder2.sensorId.setTag(Integer.valueOf(this.mSensorList.get(position).getId()));
                Utilities.addGraphSeries(holder2.graphView, this.mSensorList.get(position).getSeries());
            } else {
                Utilities.updateGraphAxis(holder2.graphView, this.mSensorList.get(position).getSeries());
            }
        }
        return convertView;
    }

    public void handlePacket(byte[] data) {
        int[] dataPacket = Utilities.signedToUnsigned(data);
        if (((dataPacket[0] >> 7) & 1) != 0) {
            int sensorId = dataPacket[0] & TransportMediator.KEYCODE_MEDIA_PAUSE;
            int sensorType = dataPacket[1];
            double timeStamp = (double) (dataPacket[2] | (dataPacket[3] << 8) | (dataPacket[4] << 16) | (dataPacket[5] << 24));
            Iterator<MainSensor> it = this.mSensorList.iterator();
            while (it.hasNext()) {
                MainSensor sensor = it.next();
                if (sensor.getId() == sensorId) {
                    sensor.updateSeries(timeStamp, data);
                    notifyDataSetChanged();
                    return;
                }
            }
            switch (sensorType) {
                case 1:
                    this.mSensorList.add(new AccelerometerSensor(sensorId, timeStamp, data));
                    break;
                case 2:
                    this.mSensorList.add(new C0Sensor(sensorId, timeStamp, data));
                    break;
                case 3:
                    this.mSensorList.add(new TemperatureSensor(sensorId, timeStamp, data));
                    break;
                case 4:
                    this.mSensorList.add(new VisualLightSensor(sensorId, timeStamp, data));
                    break;
                case 5:
                    this.mSensorList.add(new GenericSensor(sensorId, data));
                    break;
                case 11:
                    this.mSensorList.add(new AccelerometerSensor(sensorId, timeStamp, data));
                    break;
                case 12:
                    this.mSensorList.add(new AccelerometerSensor(sensorId, timeStamp, data));
                    break;
            }
            notifyDataSetChanged();
        }
    }

    private class ViewHolder {
        GraphView graphView;
        TextView printString;
        TextView sensorId;
        TextView sensorName;
        TextView timeMessage;

        private ViewHolder() {
        }
    }
}
