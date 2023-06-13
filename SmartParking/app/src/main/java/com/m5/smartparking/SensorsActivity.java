package com.m5.smartparking;


import android.app.Activity;
import android.os.Bundle;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.Toast;



public class SensorsActivity extends Activity implements SensorEventListener, OnCheckedChangeListener {


    private final static float ACC = 13;
    private CheckBox           switchButton;
    private TextView            textBarrera;
   // private TextView            textLugar1;

    private SensorManager      sensor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sensors);

        sensor = (SensorManager) getSystemService(SENSOR_SERVICE);

        textBarrera = findViewById(R.id.barreraOcupada);
     //   textLugar1 = findViewById(R.id.lugarOcupado1);
        switchButton = findViewById(R.id.checkBox);
        switchButton.setOnCheckedChangeListener(this);

    }


    @Override
    protected void onResume()
    {
        super.onResume();

        if (switchButton.isChecked())
        {
            registerSenser();
        }

    }

    @Override
    protected void onStop()
    {
        unregisterSenser();
        super.onStop();
    }

    @Override
    protected void onPause()
    {
        if (switchButton.isChecked())
        {
            unregisterSenser();
        }
        super.onPause();
    }




    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy)
    {

    }

    @Override
    public void onSensorChanged(SensorEvent event)
    {

        int sensorType = event.sensor.getType();

        float[] values = event.values;

        if (sensorType == Sensor.TYPE_ACCELEROMETER)
        {
            if ((Math.abs(values[0]) > ACC || Math.abs(values[1]) > ACC || Math.abs(values[2]) > ACC))
            {
                Log.i("sensor", "running");
                textBarrera.setText(getText(R.string.openBarrier));
                textBarrera.setBackgroundResource(R.color.green);
                // while ()
                // textBarrera.setText("Cerrada");
                // textBarrera.setBackgroundResource(R.color.red)
            }
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
    {
        if (isChecked)
        {
            registerSenser();
        }
        else
        {
            unregisterSenser();
        }
    }

    private void registerSenser()
    {
        boolean done;
        done = sensor.registerListener(this, sensor.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_NORMAL);

        if (!done)
        {
            Toast.makeText(this, "Sensor no disponible", Toast.LENGTH_SHORT).show();
            switchButton.setChecked(false);
        }
        Log.i("sensor", "register");
    }

    private void unregisterSenser()
    {
        sensor.unregisterListener(this);
        Log.i("sensor", "unregister");
    }
}