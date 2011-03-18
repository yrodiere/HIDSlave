package net.hidroid;

import net.hidroid.l2cap.L2capSocket;
import net.hidroid.l2cap.L2capStreamSocket;
import android.bluetooth.BluetoothDevice;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

class L2capTester extends Thread {
	final BluetoothDevice device;
	final int psm;
	final String text;

	final Handler guiHandler;

	public L2capTester(BluetoothDevice device, int psm, String text, Handler guiHandler) {
		super();
		this.device = device;
		this.psm = psm;
		this.text = text;
		this.guiHandler = guiHandler;
	}

	public void run() {
		L2capSocket socket = new L2capStreamSocket();
		try {
			byte buffer[] = new byte[1024];
			int nRead = 0;

			socket.connect(device, psm, 0);
			writeLog("\n" + "Connected!");

			socket.getOutputStream().write(text.getBytes());
			writeLog("\n" + "Written!");
			writeLog("\n" + "Waiting...");

			Thread.sleep(200); // Wait for a response
			nRead = socket.getInputStream().read(buffer);
			writeLog("\n" + String.format("Read %d bytes!", nRead));
			writeOutput("\nResponse: " + new String(buffer, 0, nRead));
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
		guiBdl.putString(TestActivity.KEY_USER_OUTPUT, string);
		guiMsg.setData(guiBdl);
		guiMsg.sendToTarget();
	}

	private void writeLog(String string) {
		Bundle guiBdl = new Bundle();
		Message guiMsg = Message.obtain(guiHandler);
		guiBdl.putString(TestActivity.KEY_LOG, string);
		guiMsg.setData(guiBdl);
		guiMsg.sendToTarget();
	}
}