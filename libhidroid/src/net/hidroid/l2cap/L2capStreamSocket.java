package net.hidroid.l2cap;

import java.io.IOException;

import android.bluetooth.BluetoothDevice;

public class L2capStreamSocket extends L2capSocket {
	@Override
	protected Type getSocketType() {
		return L2capSocket.Type.STREAM;
	}

	@Override
	public void connect(BluetoothDevice remoteDevice, int remotePort,
			int timeout) throws IOException {
		super.connect(remoteDevice, remotePort, timeout);
		inputStream = new L2capInputStream(nativeSocket);
		outputStream = new L2capOutputStream(nativeSocket);
	}
}
