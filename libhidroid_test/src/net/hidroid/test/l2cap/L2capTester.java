package net.hidroid.test.l2cap;

import net.hidroid.l2cap.L2capSocket;

import java.util.Arrays;
import android.bluetooth.BluetoothDevice;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

class L2capTester extends Thread {
	private L2capSocket socket;
	private final BluetoothDevice device;
	private final int psm;
	private final byte[] message;
	private final boolean waitForResponse;

	final Handler guiHandler;

	public L2capTester(L2capSocket socket, BluetoothDevice device, int psm,
			byte[] message, boolean waitForResponse, Handler guiHandler) {
		super();
		this.socket = socket;
		this.device = device;
		this.psm = psm;
		this.message = message;
		this.waitForResponse = waitForResponse;
		this.guiHandler = guiHandler;
	}

	public void run() {
		try {
			byte buffer[] = new byte[1024];
			int nRead = 0;

			socket.connect(device, psm, 0);
			writeLog("\n" + "Connected!");

			socket.getOutputStream().write(message);
			writeLog("\n" + "Written!");
			writeOutput("\nSent: " + Arrays.toString(message));

			if (waitForResponse) {
				writeLog("\n" + "Waiting...");
				Thread.sleep(200); // Wait for a response
				nRead = socket.getInputStream().read(buffer);
				writeLog("\n" + String.format("Read %d bytes!", nRead));
				writeOutput("\nResponse: " + new String(buffer, 0, nRead));
			}
		} catch (Exception e) {
			writeLog("\nEXCEPTION CAUGHT while running: " + e.getMessage());
		} finally {
			try {
				socket.close();
			} catch (Exception e) {
				writeLog("\nEXCEPTION CAUGHT while closing: " + e.getMessage());
			}
		}
	}

	private void writeOutput(String string) {
		Bundle guiBdl = new Bundle();
		Message guiMsg = Message.obtain(guiHandler);
		guiBdl.putString(L2capTestActivity.KEY_USER_OUTPUT, string);
		guiMsg.setData(guiBdl);
		guiMsg.sendToTarget();
	}

	private void writeLog(String string) {
		Bundle guiBdl = new Bundle();
		Message guiMsg = Message.obtain(guiHandler);
		guiBdl.putString(L2capTestActivity.KEY_LOG, string);
		guiMsg.setData(guiBdl);
		guiMsg.sendToTarget();
	}
}