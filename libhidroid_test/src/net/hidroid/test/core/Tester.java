package net.hidroid.test.core;

import net.hidroid.l2cap.L2capSocket;

import android.bluetooth.BluetoothDevice;
import android.util.Log;

public abstract class Tester {
	private final String logTag;

	// private Handler guiHandler;

	protected Tester(String logTag) {
		super();
		this.logTag = logTag;
		// this.guiHandler = new Handler();
	}

	public abstract PsmListener getPsmListener();

	public interface PsmListener {
		public void onPsmChanged(int newPsm);
	}

	public abstract MessageListener getMessageListener();

	public interface MessageListener {
		public void onMessageChanged(byte[] newMessage);
	}

	public abstract SocketTypeListener getSocketTypeListener();

	public interface SocketTypeListener {
		public void onSocketTypeChanged(
				Class<? extends L2capSocket> newSocketType);
	}

	public void runTest(final BluetoothDevice remoteDevice) {
		Thread testThread = new Thread() {
			public void run() {
				try {
					doTest(remoteDevice);
				} catch (Exception e) {
					Log.e(logTag,
							"EXCEPTION CAUGHT while running:\n"
									+ Log.getStackTraceString(e));
				} finally {
					try {
						cleanUp();
					} catch (Exception e) {
						Log.e(logTag,
								"EXCEPTION CAUGHT while closing:\n"
										+ Log.getStackTraceString(e));
					}
				}
			}
		};

		testThread.start();
	}

	protected abstract void doTest(final BluetoothDevice remoteDevice)
			throws Exception;

	protected abstract void cleanUp() throws Exception;

	protected void log(String logMessage) {
		Log.v(logTag, logMessage);
	}
}