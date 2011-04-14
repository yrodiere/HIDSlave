package net.hidroid.test.core;

import java.util.Arrays;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.SynchronousQueue;

import net.hidroid.hidp.HidpSockOptSetter;
import net.hidroid.l2cap.L2capSocket;
import android.bluetooth.BluetoothDevice;

public class KeyboardHidTester extends Tester {
	protected L2capSocket ctrlSocket = null;
	protected L2capSocket intrSocket = null;
	protected BlockingQueue<Byte> input = null;

	public KeyboardHidTester(String logTag) {
		super(logTag);
		this.input = new SynchronousQueue<Byte>();
	}

	@Override
	public PsmListener getPsmListener() {
		return null;
	}

	@Override
	public MessageListener getMessageListener() {
		return new MessageListener() {

			@Override
			public void onMessageChanged(byte[] newMessage) {
				if (newMessage.length > 0) {
					HIDASCIITranslator trans = new HIDASCIITranslator();
					Byte keycode = new Byte(
							trans.Translate(newMessage[newMessage.length - 1]));
					input.offer(keycode);
				}
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
		final int KEYCODE_POS = 4;
		byte[] message = { (byte) 0xa1, 0x01, 0x00, 0x00, 0x0f, 0x00, 0x00,
				0x00, 0x00, 0x00 };

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

		while (true) {
			// Send "key pressed" message
			message[KEYCODE_POS] = input.take();
			log("Trying to send: " + Arrays.toString(message));
			intrSocket.getOutputStream().write(message);
			log("Sent!");

			// Send "key released" message
			message[KEYCODE_POS] = 0;
			log("Trying to send: " + Arrays.toString(message));
			intrSocket.getOutputStream().write(message);
			log("Sent!");
		}
	}

	@Override
	protected void cleanUp() throws Exception {
		intrSocket.close();
		ctrlSocket.close();
	}

}
