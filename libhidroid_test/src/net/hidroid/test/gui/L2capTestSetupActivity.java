package net.hidroid.test.gui;

import java.util.Set;

import net.hidroid.l2cap.L2capDatagramSocket;
import net.hidroid.l2cap.L2capSeqPacketSocket;
import net.hidroid.l2cap.L2capStreamSocket;
import net.hidroid.test.R;
import net.hidroid.test.core.KeyboardHidTester;
import net.hidroid.test.core.L2capTester;
import net.hidroid.test.core.RawHidTester;
import net.hidroid.test.core.Tester;
import net.hidroid.test.core.Tester.MessageListener;
import net.hidroid.test.core.Tester.PsmListener;
import net.hidroid.test.core.Tester.SocketTypeListener;

import android.app.Activity;
import android.widget.AdapterView;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Pair;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.Toast;

public class L2capTestSetupActivity extends Activity {
	private Tester tester = null;

	private EditText testInput = null;
	private EditText psm = null;
	private RadioGroup socketType = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.setup);

		testInput = (EditText) findViewById(R.id.testInput);
		socketType = (RadioGroup) findViewById(R.id.socketType);
		psm = (EditText) findViewById(R.id.psm);
		Button doTest = (Button) findViewById(R.id.doTest);
		final Spinner testType = (Spinner) findViewById(R.id.testType);

		// Set up remote device choice dialog
		AlertDialog.Builder chooseRemoteDeviceAlertBuilder = new AlertDialog.Builder(
				this);
		chooseRemoteDeviceAlertBuilder
				.setTitle(getString(R.string.chooseBondedDevice));
		doTest.setOnClickListener(new OnTestRequestListener(
				chooseRemoteDeviceAlertBuilder));

		// Set test types
		Tester[] testers = new Tester[testType.getCount()];
		testers[0] = new L2capTester(getString(R.string.test_log));
		testers[1] = new RawHidTester(getString(R.string.test_log));
		testers[2] = new KeyboardHidTester(getString(R.string.test_log));
		// TODO : add other testers

		testType.setOnItemSelectedListener(new OnTestTypeSelectedListener(
				testers));
	}

	private class OnTestTypeSelectedListener implements
			AdapterView.OnItemSelectedListener {
		private Tester[] testers;
		private OnMessageChangedListener messageWatcher = null;
		private OnPsmChangedListener psmWatcher = null;

		public OnTestTypeSelectedListener(Tester[] testers) {
			super();
			this.testers = testers;
		}

		public void onItemSelected(AdapterView<?> parent, View view, int pos,
				long id) {
			if (pos < testers.length) {
				tester = testers[pos];

				testInput.removeTextChangedListener(messageWatcher);
				MessageListener msgListn = tester.getMessageListener();
				if (msgListn != null) {
					testInput.setEnabled(true);
					messageWatcher = new OnMessageChangedListener(msgListn);
					testInput.addTextChangedListener(messageWatcher);
					messageWatcher.afterTextChanged(testInput.getText());
				} else {
					testInput.setEnabled(false);
					messageWatcher = null;
				}

				psm.removeTextChangedListener(psmWatcher);
				PsmListener psmListener = tester.getPsmListener();
				if (psmListener != null) {
					psm.setEnabled(true);
					psmWatcher = new OnPsmChangedListener(psmListener);
					psm.addTextChangedListener(psmWatcher);
					psmWatcher.afterTextChanged(psm.getText());
				} else {
					psm.setEnabled(false);
					psmWatcher = null;
				}

				SocketTypeListener socketTypeListener = tester
						.getSocketTypeListener();
				if (socketTypeListener != null) {
					OnSocketTypeChangedListener listener = new OnSocketTypeChangedListener(
							socketTypeListener);
					socketType.setOnCheckedChangeListener(listener);
					// Update the tester using the new listener
					listener.onCheckedChanged(socketType, socketType
							.getCheckedRadioButtonId());
				} else {
					socketType.setOnCheckedChangeListener(null);
				}
			}
		}

		public void onNothingSelected(AdapterView<?> parent) {
			// Do nothing.
		}
	}

	private class OnMessageChangedListener implements TextWatcher {
		private MessageListener messageListener;

		public OnMessageChangedListener(MessageListener messageListener) {
			super();
			this.messageListener = messageListener;
		}

		@Override
		public void afterTextChanged(Editable s) {
			messageListener.onMessageChanged(s.toString().getBytes());
		}

		@Override
		public void beforeTextChanged(CharSequence s, int start, int count,
				int after) {
			// Do nothing
		}

		@Override
		public void onTextChanged(CharSequence s, int start, int before,
				int count) {
			// Do nothing
		}
	}

	private class OnPsmChangedListener implements TextWatcher {
		private PsmListener psmListener;

		public OnPsmChangedListener(PsmListener psmListener) {
			super();
			this.psmListener = psmListener;
		}

		@Override
		public void afterTextChanged(Editable s) {
			psmListener.onPsmChanged(Integer.parseInt(s.toString()));
		}

		@Override
		public void beforeTextChanged(CharSequence s, int start, int count,
				int after) {
			// Do nothing
		}

		@Override
		public void onTextChanged(CharSequence s, int start, int before,
				int count) {
			// Do nothing
		}
	}

	private class OnSocketTypeChangedListener implements
			RadioGroup.OnCheckedChangeListener {
		private SocketTypeListener socketTypeListener;

		public OnSocketTypeChangedListener(SocketTypeListener socketTypeListener) {
			super();
			this.socketTypeListener = socketTypeListener;
		}

		@Override
		public void onCheckedChanged(RadioGroup group, int checkedId) {
			switch (socketType.getCheckedRadioButtonId()) {
			case R.id.streamingType:
				socketTypeListener.onSocketTypeChanged(L2capStreamSocket.class);
				break;
			case R.id.datagramType:
				socketTypeListener
						.onSocketTypeChanged(L2capDatagramSocket.class);
				break;
			case R.id.seqpacketType:
				socketTypeListener
						.onSocketTypeChanged(L2capSeqPacketSocket.class);
				break;
			}
		}
	}

	private class OnTestRequestListener implements View.OnClickListener {
		AlertDialog.Builder builder;

		public OnTestRequestListener(Builder builder) {
			super();
			this.builder = builder;
		}

		private class ChosenDeviceListener implements
				DialogInterface.OnClickListener {
			BluetoothDevice[] devices;

			public ChosenDeviceListener(BluetoothDevice[] devices) {
				super();
				this.devices = devices;
			}

			public void onClick(DialogInterface dialogInterface, int item) {
				tester.runTest(devices[item]);
			}
		}

		public void onClick(View v) {
			Pair<CharSequence[], BluetoothDevice[]> bondedDevices = getBondedDevices();

			if (bondedDevices != null) {
				builder.setItems(bondedDevices.first, new ChosenDeviceListener(
						bondedDevices.second));
				builder.create().show();
			}
		}

		private Pair<CharSequence[], BluetoothDevice[]> getBondedDevices() {
			BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
			if (adapter == null) {
				Toast
						.makeText(
								getApplicationContext(),
								"Bluetooth does not seem to be supported on this device.",
								Toast.LENGTH_LONG).show();
				return null;
			}

			Set<BluetoothDevice> devicesSet = adapter.getBondedDevices();
			if (devicesSet.isEmpty()) {
				Toast
						.makeText(
								getApplicationContext(),
								"Bluetooth is disabled, or no bonded device is available",
								Toast.LENGTH_LONG).show();
				return null;
			}

			BluetoothDevice[] devices = new BluetoothDevice[devicesSet.size()];
			devices = devicesSet.toArray(devices);
			final CharSequence[] labels = new CharSequence[devices.length];
			for (int i = 0; i < devices.length; ++i) {
				labels[i] = devices[i].getName();
			}

			return Pair.create(labels, devices);
		}
	}
}
