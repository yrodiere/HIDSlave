package net.hidroid.l2cap;

import java.io.IOException;

import android.bluetooth.BluetoothDevice;

public class L2capStreamSocket extends L2capSocket {
	@Override
	protected native int getSocketType();

	@Override
	public void connect(BluetoothDevice remoteDevice, int psm,
			int timeout) throws IOException {
		super.connect(remoteDevice, psm, timeout);
		inputStream = new L2capInputStream(nativeSocket);
		outputStream = new L2capOutputStream(nativeSocket);
	}

	static {
		System.loadLibrary("hidroid");
	}
}
