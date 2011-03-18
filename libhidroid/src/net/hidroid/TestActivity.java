package net.hidroid;

import net.hidroid.l2cap.L2capSocket;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class TestActivity extends Activity {
	private static final String KEY_USER_INPUT = "userinput";
	private static final String KEY_USER_OUTPUT = "useroutput";
	private static final String KEY_LOG = "log";
	private EditText testInput = null;
	private TextView testOutput = null;
	private TextView testLog = null;
	private HandlerThread l2capTestThread = null;
	private Handler l2capTester = null;

	private class L2capTester extends Handler {
		final Handler guiHandler;

		public L2capTester(Looper looper, Handler guiHandler) {
			super(looper);
			this.guiHandler = guiHandler;
		}

		public void handleMessage(Message msg) {
			final String text = msg.getData().getString(KEY_USER_INPUT);

			L2capSocket socket = new L2capSocket();
			try {
				byte buffer[] = new byte[1024];
				int nRead = 0;

				socket.connect(BluetoothAdapter.getDefaultAdapter()
						.getRemoteDevice(getString(R.string.testBluetoothAddress)), 0);
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
			guiBdl.putString(KEY_USER_OUTPUT, string);
			guiMsg.setData(guiBdl);
			guiMsg.sendToTarget();
		}

		private void writeLog(String string) {
			Bundle guiBdl = new Bundle();
			Message guiMsg = Message.obtain(guiHandler);
			guiBdl.putString(KEY_LOG, string);
			guiMsg.setData(guiBdl);
			guiMsg.sendToTarget();
		}
	};

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
		l2capTester = new L2capTester(l2capTestThread.getLooper(),
				new TextViewsUpdater());

		doTest.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				// testOutput.setText(new L2capSocket().test());
				Message msg = Message.obtain(l2capTester);
				Bundle bdl = new Bundle();

				testOutput.setText("");
				testLog.setText("");
				bdl.putString(KEY_USER_INPUT, testInput.getText().toString());
				msg.setData(bdl);
				msg.sendToTarget();
			}
		});
	}
}