package net.hidroid;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class TestActivity extends Activity {
	static final String KEY_USER_OUTPUT = "useroutput";
	static final String KEY_LOG = "log";

	private EditText testInput = null;
	private TextView testOutput = null;
	private TextView testLog = null;
	private HandlerThread l2capTestThread = null;
	private Handler l2capTester = null;

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

	private class OnTestRequestListener implements View.OnClickListener {
		public void onClick(View v) {
			// testOutput.setText(new L2capSocket().test());
			Message msg = Message.obtain(l2capTester);
			Bundle bdl = new Bundle();
			
			testOutput.setText("");
			testLog.setText("");
			bdl.putString(L2capTester.KEY_REMOTE_ADDRESS,
					getString(R.string.testBluetoothAddress));
			bdl.putString(L2capTester.KEY_USER_INPUT, testInput.getText()
					.toString());
			msg.setData(bdl);
			msg.sendToTarget();
		}
	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		final Button doTest = (Button) findViewById(R.id.do_test);
		testInput = (EditText) findViewById(R.id.test_input);
		testOutput = (TextView) findViewById(R.id.test_output);
		testLog = (TextView) findViewById(R.id.test_log);

		l2capTestThread = new HandlerThread("L2CAP tester");
		l2capTestThread.start();
		l2capTester = new L2capTester(this, l2capTestThread.getLooper(),
				new TextViewsUpdater());

		doTest.setOnClickListener(new OnTestRequestListener());
	}
}
