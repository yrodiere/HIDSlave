package net.hidroid;

import android.app.Activity;
import android.os.Bundle;
//import android.text.Editable;
import android.view.View;
import android.widget.Button;
//import android.widget.EditText;
import android.widget.TextView;

public class TestActivity extends Activity {

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		final Button doTest = (Button) findViewById(R.id.do_test);
		//final EditText testInput = (EditText) findViewById(R.id.test_input);
		final TextView testOutput = (TextView) findViewById(R.id.test_output);
		
		doTest.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				//final Editable text = testInput.getText();
				testOutput.setText(new L2capSocket().test());
			}
		});

	}
}