package net.hidroid.test.l2cap;

import java.util.Set;

import net.hidroid.l2cap.L2capDatagramSocket;
import net.hidroid.l2cap.L2capSeqPacketSocket;
import net.hidroid.l2cap.L2capStreamSocket;
import net.hidroid.test.R;

import android.app.Activity;
import android.widget.AdapterView;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Pair;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.Toast;

public class L2capTestSetupActivity extends Activity {
	private byte[][] l2capStaticMessages = { { (byte) 0xa1, 0x01, 0x00, 0x00,
			48, 0x00, 0x00, 0x00, 0x00, 0x00 }
	// This array's length MUST equal l2capMessageType's length minus one.
	};
	private EditText testInput = null;
	private RadioGroup socketType = null;
	private EditText psm = null;
	private Button doTest = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.setup);

		testInput = (EditText) findViewById(R.id.testInput);
		socketType = (RadioGroup) findViewById(R.id.socketType);
		psm = (EditText) findViewById(R.id.psm);
		doTest = (Button) findViewById(R.id.doTest);
		final Spinner l2capMessageType = (Spinner) findViewById(R.id.l2capMessageType);

		// Set up remote device choice dialog
		AlertDialog.Builder chooseRemoteDeviceAlertBuilder = new AlertDialog.Builder(
				this);
		chooseRemoteDeviceAlertBuilder
				.setTitle(getString(R.string.chooseBondedDevice));

		// Set test types
		OnTestRequestListener[] onTestRequestListeners = new OnTestRequestListener[l2capMessageType
				.getCount()];
		onTestRequestListeners[0] = new OnUserMessageTestRequestListener(
				chooseRemoteDeviceAlertBuilder);
		for (int i = 1; i < l2capMessageType.getCount(); ++i) {
			onTestRequestListeners[i] = new OnStaticMessageTestRequestListener(
					chooseRemoteDeviceAlertBuilder, l2capStaticMessages[i - 1]);
		}

		l2capMessageType
				.setOnItemSelectedListener(new OnL2capMessageTypeSelectedListener(
						onTestRequestListeners));
	}

	public class OnL2capMessageTypeSelectedListener implements
			AdapterView.OnItemSelectedListener {
		private OnTestRequestListener[] onTestRequestListeners;

		public OnL2capMessageTypeSelectedListener(
				OnTestRequestListener[] onTestRequestListeners) {
			super();
			this.onTestRequestListeners = onTestRequestListeners;
		}

		public void onItemSelected(AdapterView<?> parent, View view, int pos,
				long id) {
			Toast.makeText(parent.getContext(), Long.toString(id),
					Toast.LENGTH_LONG).show();
			if (pos < onTestRequestListeners.length) {
				doTest.setOnClickListener(onTestRequestListeners[pos]);
				if (pos == 0) {
					testInput.setEnabled(true);
				} else {
					testInput.setEnabled(false);
				}
			}
		}

		public void onNothingSelected(AdapterView<?> parent) {
			// Do nothing.
		}
	}

	private abstract class OnTestRequestListener implements
			View.OnClickListener {
		AlertDialog.Builder builder;

		public OnTestRequestListener(Builder builder) {
			super();
			this.builder = builder;
		}

		private class ChosenDeviceListener implements
				DialogInterface.OnClickListener {
			Intent onClickIntent;
			BluetoothDevice[] devices;

			public ChosenDeviceListener(Intent onClickIntent,
					BluetoothDevice[] devices) {
				super();
				this.onClickIntent = onClickIntent;
				this.devices = devices;
			}

			public void onClick(DialogInterface dialogInterface, int item) {
				onClickIntent.putExtra(
						L2capTestActivity.KEY_BLUETOOTH_REMOTE_DEVICE,
						devices[item]);
				startActivity(onClickIntent);
				return;
			}
		}

		public void onClick(View v) {
			Class<?> socketClass = getSelectedSocketType();
			Pair<CharSequence[], BluetoothDevice[]> bondedDevices = getBondedDevices();

			if (socketClass != null && bondedDevices != null) {
				Intent intent = new Intent(getApplicationContext(),
						L2capTestActivity.class);
				intent.putExtra(L2capTestActivity.KEY_BLUETOOTH_PSM,
						Integer.parseInt(psm.getText().toString()));
				intent.putExtra(L2capTestActivity.KEY_BLUETOOTH_WRITE_STRING,
						getL2capMessage());
				intent.putExtra(L2capTestActivity.KEY_WAIT_RESPONSE,
						getWaitForResponse());
				intent.putExtra(L2capTestActivity.KEY_L2CAP_SOCKET, socketClass);

				builder.setItems(bondedDevices.first, new ChosenDeviceListener(
						intent, bondedDevices.second));
				builder.create().show();
			}
		}

		protected abstract byte[] getL2capMessage();

		protected abstract boolean getWaitForResponse();

		private Class<?> getSelectedSocketType() {
			switch (socketType.getCheckedRadioButtonId()) {
			case R.id.streamingType:
				return L2capStreamSocket.class;
			case R.id.datagramType:
				return L2capDatagramSocket.class;
			case R.id.seqpacketType:
				return L2capSeqPacketSocket.class;
			default:
				Toast.makeText(getApplicationContext(),
						"No L2CAP socket type selected.", Toast.LENGTH_SHORT)
						.show();
				return null;
			}
		}

		private Pair<CharSequence[], BluetoothDevice[]> getBondedDevices() {
			BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
			if (adapter == null) {
				Toast.makeText(
						getApplicationContext(),
						"Bluetooth does not seem to be supported on this device.",
						Toast.LENGTH_LONG).show();
				return null;
			}

			Set<BluetoothDevice> devicesSet = adapter.getBondedDevices();
			if (devicesSet.isEmpty()) {
				Toast.makeText(
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

	private class OnUserMessageTestRequestListener extends
			OnTestRequestListener {
		public OnUserMessageTestRequestListener(Builder builder) {
			super(builder);
		}

		@Override
		protected byte[] getL2capMessage() {
			return testInput.getText().toString().getBytes();
		}

		@Override
		protected boolean getWaitForResponse() {
			return true;
		}
	}

	private class OnStaticMessageTestRequestListener extends
			OnTestRequestListener {
		byte[] message;

		public OnStaticMessageTestRequestListener(Builder builder,
				byte[] message) {
			super(builder);
			this.message = message;
		}

		@Override
		protected byte[] getL2capMessage() {
			return message;
		}

		@Override
		protected boolean getWaitForResponse() {
			return false;
		}
	}
}
