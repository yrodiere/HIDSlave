package net.hidroid.l2cap;

import java.io.IOException;

import android.bluetooth.BluetoothDevice;

public class L2capSeqPacketSocket extends L2capSocket {
	@Override
	protected native int getSocketType();

	@Override
	public void connect(BluetoothDevice remoteDevice, int psm,
			int timeout) throws IOException {
		super.connect(remoteDevice, psm, timeout);
		inputStream = null;
		outputStream = new L2capOutputStream(nativeSocket);
	}

	static {
		System.loadLibrary("hidroid");
	}
}
