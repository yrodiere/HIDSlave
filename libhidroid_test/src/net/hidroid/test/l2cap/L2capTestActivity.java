package net.hidroid.test.l2cap;

import java.lang.reflect.InvocationTargetException;
import java.security.InvalidParameterException;

import net.hidroid.l2cap.L2capSocket;
import net.hidroid.test.R;
import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.TextView;

public class L2capTestActivity extends Activity {
	static final String KEY_L2CAP_SOCKET = "l2capSocket";
	static final String KEY_BLUETOOTH_WRITE_STRING = "written";
	static final String KEY_BLUETOOTH_REMOTE_DEVICE = "remoteDevice";
	static final String KEY_BLUETOOTH_PSM = "psm";
	static final String KEY_WAIT_RESPONSE = "waitForResponse";

	static final String KEY_USER_OUTPUT = "userOutput";
	static final String KEY_LOG = "log";

	private TextView testOutput = null;
	private TextView testLog = null;
	private L2capTester l2capTestThread = null;

	private class TextViewsUpdater extends Handler {
		public void handleMessage(Message msg) {
			final String output = msg.getData().getString(KEY_USER_OUTPUT);
			final String log = msg.getData().getString(KEY_LOG);

			if (output != null) {
				testOutput.append(output);
			}
			if (log != null) {
				testLog.append(log);
			}
		}
	};

	@Override
	public void onCreate(Bundle savedInstanceState) {
		try {
			super.onCreate(savedInstanceState);
			setContentView(R.layout.test);

			testOutput = (TextView) findViewById(R.id.testOutput);
			testLog = (TextView) findViewById(R.id.testLog);

			Bundle param = this.getIntent().getExtras();

			l2capTestThread = new L2capTester(
					getSocketFromClass(param.get(KEY_L2CAP_SOCKET)),
					(BluetoothDevice) param.get(KEY_BLUETOOTH_REMOTE_DEVICE),
					param.getInt(KEY_BLUETOOTH_PSM),
					param.getByteArray(KEY_BLUETOOTH_WRITE_STRING),
					param.getBoolean(KEY_WAIT_RESPONSE),
					new TextViewsUpdater());
			l2capTestThread.start();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * @param socketClassParam
	 *            The class that should be instantiated by this method.
	 * @return the non-null object of type L2capSocket instantiating the
	 *         socketClassParam class.
	 * @throws IllegalArgumentException
	 *             If socketClassParam is not a class object representing a
	 *             subclass of L2capSocket.
	 * @throws SecurityException
	 *             If something is wrong with sockets implementation.
	 * @throws InstantiationException
	 *             If something is wrong with sockets implementation.
	 * @throws IllegalAccessException
	 *             If something is wrong with sockets implementation.
	 * @throws InvocationTargetException
	 *             If something is wrong with sockets implementation.
	 * @throws NoSuchMethodException
	 *             If something is wrong with sockets implementation.
	 */
	private L2capSocket getSocketFromClass(Object socketClassParam)
			throws IllegalArgumentException, SecurityException,
			InstantiationException, IllegalAccessException,
			InvocationTargetException, NoSuchMethodException {
		L2capSocket socket = null;
		if (socketClassParam instanceof Class<?>) {
			Class<? extends L2capSocket> socketClass = ((Class<?>) socketClassParam)
					.asSubclass(L2capSocket.class);
			if (socketClass != null) {
				socket = socketClass.getConstructor().newInstance();
			}
		}

		if (socket == null) {
			throw new InvalidParameterException(
					"Parameter for key KEY_L2CAP_SOCKET is not a valid 'Class<? extends L2capSocket'");
		}

		return socket;
	}
}