package net.hidroid;

import net.hidroid.l2cap.L2capSocket;
import net.hidroid.l2cap.L2capStreamSocket;
import android.bluetooth.BluetoothAdapter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

class L2capTester extends Handler {
	static final String KEY_REMOTE_ADDRESS = "address";
	static final String KEY_USER_INPUT = "userinput";

	final Handler guiHandler;

	public L2capTester(TestActivity testActivity, Looper looper,
			Handler guiHandler) {
		super(looper);
		this.guiHandler = guiHandler;
	}

	public void handleMessage(Message msg) {
		final String address = msg.getData().getString(KEY_REMOTE_ADDRESS);
		final String text = msg.getData().getString(KEY_USER_INPUT);

		L2capSocket socket = new L2capStreamSocket();
		try {
			byte buffer[] = new byte[1024];
			int nRead = 0;

			socket.connect(BluetoothAdapter.getDefaultAdapter()
					.getRemoteDevice(address), 0x1001, 0);
			writeLog("\n" + "Connected!");

			socket.getOutputStream().write(text.getBytes());
			writeLog("\n" + "Written!");
			writeLog("\n" + "Waiting...");

			Thread.sleep(200); // Wait for a response
			nRead = socket.getInputStream().read(buffer);
			writeLog("\n" + String.format("Read %d bytes!", nRead));
			writeOutput("\nResponse: " + new String(buffer, 0, nRead));
		} catch (Exception e) {
			writeLog("\nEXCEPTION CAUGHT: " + e.getMessage());
		} finally {
			try {
				socket.close();
			} catch (Exception e) {
				writeLog("\nEXCEPTION CAUGHT: " + e.getMessage());
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