package net.hidroid;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.TextView;

public class TestActivity extends Activity {
	static final String KEY_BLUETOOTH_WRITE_STRING = "written";
	static final String KEY_BLUETOOTH_REMOTE_DEVICE = "remoteDevice";
	static final String KEY_BLUETOOTH_PSM = "psm";

	static final String KEY_USER_OUTPUT = "useroutput";
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
		super.onCreate(savedInstanceState);
		setContentView(R.layout.test);

		testOutput = (TextView) findViewById(R.id.testOutput);
		testLog = (TextView) findViewById(R.id.testLog);
		// testOutput.setText(""); // TODO: Useless ?
		// testLog.setText(""); // TODO: Useless ?

		Bundle param = this.getIntent().getExtras();
		l2capTestThread = new L2capTester(
				(BluetoothDevice) param.get(KEY_BLUETOOTH_REMOTE_DEVICE),
				param.getInt(KEY_BLUETOOTH_PSM),
				param.getString(KEY_BLUETOOTH_WRITE_STRING),
				new TextViewsUpdater());
		l2capTestThread.start();
	}
}
