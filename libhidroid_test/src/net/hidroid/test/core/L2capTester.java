package net.hidroid.test.core;

import java.util.Arrays;

import net.hidroid.l2cap.L2capSocket;
import android.bluetooth.BluetoothDevice;

public class L2capTester extends Tester {
	protected L2capSocket socket = null;
	protected int psm = -1;
	protected byte[] message = null;

	public L2capTester(String logTag) {
		super(logTag);
	}

	@Override
	public PsmListener getPsmListener() {
		return new PsmListener() {
			@Override
			public void onPsmChanged(int newPsm) {
				psm = newPsm;
			}
		};
	}

	@Override
	public MessageListener getMessageListener() {
		return new MessageListener() {
			@Override
			public void onMessageChanged(byte[] newMessage) {
				message = newMessage;
			}
		};
	}

	@Override
	public SocketTypeListener getSocketTypeListener() {
		return new SocketTypeListener() {
			@Override
			public void onSocketTypeChanged(
					Class<? extends L2capSocket> newSocketType) {
				try {
					socket = newSocketType.getConstructor().newInstance();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		};
	}

	@Override
	public void doTest(final BluetoothDevice remoteDevice) throws Exception {
		byte buffer[] = new byte[1024];
		int nRead = 0;

		log("Using socket: " + socket.toString());
		log(String.format("Trying to connect on device %s, psm %d...",
				remoteDevice.toString(), psm));
		socket.connect(remoteDevice, psm, 0);
		log("Connected!");

		log("Trying to send: " + Arrays.toString(message));
		socket.getOutputStream().write(message);
		log("Sent!");

		log("Waiting...");
		Thread.sleep(200); // Wait for a
		nRead = socket.getInputStream().read(buffer);
		log(String.format("Response (%d bytes): ", nRead)
				+ new String(buffer, 0, nRead));
	}

	@Override
	protected void cleanUp() throws Exception {
		socket.close();
	}
}
