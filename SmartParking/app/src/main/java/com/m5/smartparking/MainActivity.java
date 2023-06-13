package com.m5.smartparking;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import org.w3c.dom.Text;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button buttonSend = (Button) findViewById(R.id.buttonSend);
        Button buttonSensor = (Button) findViewById(R.id.buttonSensor);

        buttonSensor.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
               openActivity2();
            }
        });
        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                sendMessage(view);
            }
        });


    }

    public void openActivity2() {
        Intent intent = new Intent(this, SensorsActivity.class);
        startActivity(intent);
    }
    public void sendMessage(View view){
        TextView textRequest = (TextView) findViewById(R.id.request);
        TextView textResponse = (TextView) findViewById(R.id.response);
        textResponse.setText(textRequest.getText());
    }
}