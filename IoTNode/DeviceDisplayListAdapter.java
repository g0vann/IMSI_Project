package analog.com.IoTNode;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;
import java.util.ArrayList;

class DeviceDisplayListAdapter extends BaseAdapter {
    private final ArrayList<String[]> mDeviceList = new ArrayList<>();
    private LayoutInflater mLayoutInflater;

    DeviceDisplayListAdapter(Context mContext) {
        this.mLayoutInflater = LayoutInflater.from(mContext);
    }

    public int getCount() {
        return this.mDeviceList.size();
    }

    public String[] getItem(int position) {
        return this.mDeviceList.get(position);
    }

    public long getItemId(int position) {
        return (long) position;
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        if (convertView == null) {
            convertView = this.mLayoutInflater.inflate(C0009R.layout.ble_display_listview, parent, false);
            holder = new ViewHolder();
            holder.Name = (TextView) convertView.findViewById(C0009R.C0011id.name);
            holder.Rssi = (TextView) convertView.findViewById(C0009R.C0011id.rssi);
            holder.Address = (TextView) convertView.findViewById(C0009R.C0011id.address);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }
        holder.Name.setText(this.mDeviceList.get(position)[0]);
        holder.Rssi.setText(this.mDeviceList.get(position)[1]);
        holder.Address.setText(this.mDeviceList.get(position)[2]);
        return convertView;
    }

    public void addDevice(String[] deviceInfo) {
        if (deviceInfo[0] == null) {
            deviceInfo[0] = "N/A";
        }
        this.mDeviceList.add(deviceInfo);
        notifyDataSetChanged();
    }

    public void clearList() {
        this.mDeviceList.clear();
        notifyDataSetChanged();
    }

    private class ViewHolder {
        TextView Address;
        TextView Name;
        TextView Rssi;

        private ViewHolder() {
        }
    }
}
