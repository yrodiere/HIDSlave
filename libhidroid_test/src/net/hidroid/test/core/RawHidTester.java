package net.hidroid.test.core;

import java.util.Arrays;

import net.hidroid.hidp.HidpSockOptSetter;
import net.hidroid.l2cap.L2capSocket;
import android.bluetooth.BluetoothDevice;

public class RawHidTester extends Tester {
	protected L2capSocket ctrlSocket = null;
	protected L2capSocket intrSocket = null;

	public RawHidTester(String logTag) {
		super(logTag);
	}

	@Override
	public PsmListener getPsmListener() {
		return null;
	}

	@Override
	public MessageListener getMessageListener() {
		return null;
	}

	@Override
	public SocketTypeListener getSocketTypeListener() {
		return new SocketTypeListener() {
			@Override
			public void onSocketTypeChanged(
					Class<? extends L2capSocket> newSocketType) {
				try {
					ctrlSocket = newSocketType.getConstructor().newInstance();
					intrSocket = newSocketType.getConstructor().newInstance();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		};
	}

	@Override
	protected void doTest(BluetoothDevice remoteDevice) throws Exception {
		final byte[] message = { (byte) 0xa1, 0x01, 0x00, 0x00, 0x30, 0x00,
				0x00, 0x00, 0x00, 0x00 };
		int psm;

		psm = 0x11;
		log(String.format(
				"Trying to set up CTRL channel to device %s, psm %d...",
				remoteDevice.toString(), psm));
		ctrlSocket.connect(remoteDevice, psm, new HidpSockOptSetter(), 0);
		log("Connected!");

		psm = 0x13;
		log(String.format(
				"Trying to set up INTR channel to device %s, psm %d...",
				remoteDevice.toString(), psm));
		intrSocket.connect(remoteDevice, psm, new HidpSockOptSetter(), 0);
		log("Connected!");

		log("Trying to send: " + Arrays.toString(message));
		intrSocket.getOutputStream().write(message);
		log("Sent!");
	}

	@Override
	protected void cleanUp() throws Exception {
		intrSocket.close();
		ctrlSocket.close();
	}

}
