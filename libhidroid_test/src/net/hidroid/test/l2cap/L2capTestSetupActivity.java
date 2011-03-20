package net.hidroid.test.l2cap;

import java.util.Set;

import net.hidroid.test.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class L2capTestSetupActivity extends Activity {
	private EditText testInput = null;
	private EditText psm = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.setup);

		testInput = (EditText) findViewById(R.id.testInput);
		psm = (EditText) findViewById(R.id.psm);
		final Button doTest = (Button) findViewById(R.id.doTest);

		// Set up remote device choice dialog
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle(getString(R.string.chooseBondedDevice));

		doTest.setOnClickListener(new OnTestRequestListener(builder));
	}

	private class OnTestRequestListener implements View.OnClickListener {
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
			Intent intent = new Intent(getApplicationContext(),
					L2capTestActivity.class);

			intent.putExtra(L2capTestActivity.KEY_BLUETOOTH_PSM,
					Integer.parseInt(psm.getText().toString()));
			intent.putExtra(L2capTestActivity.KEY_BLUETOOTH_WRITE_STRING,
					testInput.getText().toString());

			// Show remote device chooser dialog
			Set<BluetoothDevice> devicesSet = BluetoothAdapter
					.getDefaultAdapter().getBondedDevices();
			if (devicesSet.isEmpty()) {
				Toast.makeText(
						getApplicationContext(),
						"Bluetooth is disabled, or no bonded device is available",
						Toast.LENGTH_LONG).show();
			} else {
				BluetoothDevice[] devices = new BluetoothDevice[devicesSet
						.size()];
				devices = devicesSet.toArray(devices);
				final CharSequence[] labels = new CharSequence[devices.length];
				for (int i = 0; i < devices.length; ++i) {
					labels[i] = devices[i].getName();
				}
				builder.setItems(labels, new ChosenDeviceListener(intent,
						devices));
				builder.create().show();
			}
		}
	}
}
