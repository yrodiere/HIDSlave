package net.hidroid.hidp;

import java.util.List;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;

public class BluetoothHidp extends Object implements BluetoothProfile {

	/**
	 * This is a workaround ; A2DP and HEADSET profiles are instantiated by the
	 * BluetoothAdapter class, so they don't really need this. We do, because we
	 * can't alter the BluetoothAdapter class.
	 * 
	 * @param adapter
	 */
	public BluetoothHidp(BluetoothAdapter adapter) {
		// TODO Auto-generated constructor stub
	}

	public List<BluetoothDevice> getConnectedDevices() {
		// TODO Auto-generated method stub
		return null;
	}

	public int getConnectionState(BluetoothDevice device) {
		// TODO Auto-generated method stub
		return 0;
	}

	public List<BluetoothDevice> getDevicesMatchingConnectionStates(int[] states) {
		// TODO Auto-generated method stub
		return null;
	}

	/**
	 * TODO define which objects should be passed as arguments. Some ideas :
	 * 
	 * - An OutputStream for outgoing events (key press, mouse motions, ...)
	 * 
	 * - A handler for incoming commands (?)
	 * 
	 * @return false if there is no HID host connected or on error, true
	 *         otherwise
	 */
	public boolean startInputTransmission(BluetoothDevice device,
			int TODO_OTHERS) {
		// TODO Auto-generated method stub
		return false;
	}

	/**
	 * TODO doc
	 * 
	 * @return false if there is no HID host connected or on error, true
	 *         otherwise
	 */
	public boolean stopInputTransmission(BluetoothDevice device) {
		// TODO Auto-generated method stub
		return false;
	}
}
