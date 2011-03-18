package net.hidroid.l2cap;

import java.io.IOException;

import android.bluetooth.BluetoothDevice;

public class L2capSeqPacketSocket extends L2capSocket {

	@Override
	protected Type getSocketType() {
		return L2capSocket.Type.SEQ_PAQUET;
	}

	@Override
	public void connect(BluetoothDevice remoteDevice, int remotePort,
			int timeout) throws IOException {
		super.connect(remoteDevice, remotePort, timeout);
		inputStream = null;
		outputStream = new L2capOutputStream(nativeSocket);
	}

}
