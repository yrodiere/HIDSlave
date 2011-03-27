package android.bluetooth;

import java.util.List;

/**
 * This is a workaround in order to implement BluetoothProfile interface prior
 * to its availability in Android API level 11
 * 
 * @author fenrhil
 */
public interface BluetoothProfile {

	public static final int STATE_CONNECTED = 0;
	public static final int STATE_CONNECTING = 1;
	public static final int STATE_DISCONNECTED = 2;
	public static final int STATE_DISCONNECTING = 3;

	public abstract List<BluetoothDevice> getConnectedDevices();

	public abstract int getConnectionState(BluetoothDevice device);

	public abstract List<BluetoothDevice> getDevicesMatchingConnectionStates(
			int[] states);

}
