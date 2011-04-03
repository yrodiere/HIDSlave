package net.hidroid.l2cap;

import java.io.IOException;

import android.bluetooth.BluetoothDevice;

public class L2capDatagramSocket extends L2capSocket {
	@Override
	protected native int getSocketType();

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * net.hidroid.l2cap.L2capSocket#connect(android.bluetooth.BluetoothDevice,
	 * int, net.hidroid.l2cap.L2capSocket.SockOptSetter, int)
	 * 
	 * TODO: fix input issue: read operation seems to be aborted in some way
	 * (instant return of 0).
	 */
	@Override
	public void connect(BluetoothDevice remoteDevice, int psm,
			SockOptSetter setter, int timeout) throws IOException {
		super.connect(remoteDevice, psm, setter, timeout);
		inputStream = new L2capInputStream(nativeSocket);
		outputStream = new L2capOutputStream(nativeSocket);
	}

	static {
		System.loadLibrary("hidroid");
	}
}
