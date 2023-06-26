package com.m5.smartparking;

import java.util.List;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothDevice;
import android.content.Context;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.TextView;

public class DeviceListAdapter extends BaseAdapter{
	private LayoutInflater mInflater;	
	private List<BluetoothDevice> mData;
	private OnPairButtonClickListener mListener;
	
	public DeviceListAdapter(Context context) { 
        mInflater = LayoutInflater.from(context);        
    }
	
	public void setData(List<BluetoothDevice> data) {
		mData = data;
	}
	
	public void setListener(OnPairButtonClickListener listener) {
		mListener = listener;
	}
	
	public int getCount() {
		return (mData == null) ? 0 : mData.size();
	}

	public Object getItem(int position) {
		return mData.get(position);
	}

	public long getItemId(int position) {
		return position;
	}

	@SuppressLint("MissingPermission")
	public View getView(final int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		
		if (convertView == null) {			
			convertView	=  mInflater.inflate(R.layout.list_item_device, null);
			
			holder = new ViewHolder();
			
			holder.name = (TextView) convertView.findViewById(R.id.name);
			holder.address = (TextView) convertView.findViewById(R.id.address);
			holder.pairBtn = (Button) convertView.findViewById(R.id.btn_pair);
			
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		
		BluetoothDevice device	= mData.get(position);
		
		holder.name.setText(device.getName());
		holder.address.setText(device.getAddress());
		holder.pairBtn.setText((device.getBondState() == BluetoothDevice.BOND_BONDED) ? "pair" : "UnPair");
		holder.pairBtn.setOnClickListener(new View.OnClickListener() {			
			@Override
			public void onClick(View v) {
				if (mListener != null) {
					mListener.onPairButtonClick(position);
				}
			}
		});
		
        return convertView;
	}

	static class ViewHolder {
		TextView name;
		TextView address;
		TextView pairBtn;
	}
	
	public interface OnPairButtonClickListener {
		public abstract void onPairButtonClick(int position);
	}
}