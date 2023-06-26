package com.m5.smartparking;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;

public class SmartParkingActivity extends Activity {

    private ArrayList<BluetoothDevice> mDeviceList = new ArrayList<BluetoothDevice>();

    private BluetoothAdapter mBluetoothAdapter;

    @Override
    @SuppressLint("MissingPermission")
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_smart_parking);

        Button buttonSend = (Button) findViewById(R.id.buttonSend);
        Button buttonSensor = (Button) findViewById(R.id.buttonSensor);

        //Se crea un adaptador para poder manejar el bluetooth del celular
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        buttonSensor.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                openSensorActivity();
            }
        });
        buttonSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                sendMessage(view);
            }
        });

        //se definen un broadcastReceiver que captura el broadcast del SO cuando captura los siguientes eventos:
        IntentFilter filter = new IntentFilter();

        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED); //Cambia el estado del Bluethoot (Acrtivado /Desactivado)
        filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        this.registerReceiver(mReceiver, filter);

        //se define /registra el handler que captura los broadcast anteriormente mencionados.
        registerReceiver(mReceiver, filter);
    }

    public void openSensorActivity() {
        Intent intent = new Intent(this, SensorsActivity.class);
        startActivity(intent);
    }

    // ACÁ COMUNICARNOS CON EL ARDUINO !!!
    public void sendMessage(View view){
        TextView textRequest = (TextView) findViewById(R.id.request);
        TextView textResponse = (TextView) findViewById(R.id.response);
        textResponse.setText(textRequest.getText());
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @SuppressLint("MissingPermission")
        public void onReceive(Context context, Intent intent) {
            //A través del Intent obtengo el evento de Bluetooth que informo el broadcast del SO
            String action = intent.getAction();

            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            //Si cambio de estado el Bluetooth(Activado/desactivado)
            if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                //Obtengo el parametro, aplicando un Bundle, que me indica el estado del Bluetooth
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);

                if (state != BluetoothAdapter.STATE_ON) {
                    Log.i("BLUETOOTH_BROADCAST", "BLUETOOTH DESACTIVADO");
                    showToast("Se desactivo el bluetooth");
                    Intent i = new Intent(SmartParkingActivity.this, MainActivity.class);
                    startActivity(i);
                }
            }
            else if (BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(action)) {
                Log.i("BLUETOOTH_BROADCAST", "Dispositivo remoto desconectado");
                showToast("Se desconectó dispositivo");
                Intent i = new Intent(SmartParkingActivity.this, MainActivity.class);
                startActivity(i);
            }
        }
    };

    private void showToast(String message) {
        Toast.makeText(getApplicationContext(), message, Toast.LENGTH_SHORT).show();
    }

}