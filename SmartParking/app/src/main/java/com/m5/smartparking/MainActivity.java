package com.m5.smartparking;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class MainActivity extends AppCompatActivity {

    private TextView txtStatusBluetooth;
    private Button buttonActivate;
    private Button buttonSearchDevices;
    private ArrayList<BluetoothDevice> mDeviceList = new ArrayList<BluetoothDevice>();
    private BluetoothAdapter mBluetoothAdapter;

    @Override
    @SuppressLint("MissingPermission")
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //Se definen los componentes del layout
        txtStatusBluetooth = (TextView) findViewById(R.id.statusBluetooth);
        buttonActivate = (Button) findViewById(R.id.buttonActivate);
        buttonSearchDevices = (Button) findViewById(R.id.buttonSearchDevices);

        //Se crea un adaptador para poder manejar el bluetooth del celular
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        buttonActivate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(intent, 1000);
            }
        });
        buttonSearchDevices.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();

                ArrayList<BluetoothDevice> list = new ArrayList<BluetoothDevice>();
                list.addAll(pairedDevices);
                Intent intent = new Intent(MainActivity.this, DeviceListActivity.class);
                intent.putParcelableArrayListExtra("device.list", list);
                startActivity(intent);
            }
        });

        enableComponent();
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

    protected  void enableComponent()
    {
        //se determina si existe bluetooth en el celular
        if (mBluetoothAdapter == null) {
            //si el celular no soporta bluetooth
            showUnsupported();
        } else {
            //se determina si esta activado el bluethoot
            if (mBluetoothAdapter.isEnabled()) {
                showEnabled();
            } else {
                showDisabled();
            }
        }

        //se definen un broadcastReceiver que captura el broadcast del SO cuando captura los siguientes eventos:
        IntentFilter filter = new IntentFilter();

        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED); //Cambia el estado del Bluethoot (Acrtivado /Desactivado)
        filter.addAction(BluetoothDevice.ACTION_FOUND); //Se encuentra un dispositivo bluethoot al realizar una busqueda
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED); //Cuando se comienza una busqueda de bluethoot
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED); //cuando la busqueda de bluethoot finaliza

        this.registerReceiver(mReceiver, filter);

        //se define / registra el handler que captura los broadcast anterirmente mencionados.
        registerReceiver(mReceiver, filter);
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @SuppressLint("MissingPermission")
        public void onReceive(Context context, Intent intent) {

            //A traves del Intent obtengo el evento de Bluetooth que informo el broadcast del SO
            String action = intent.getAction();

            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            //Si cambio de estado el Bluetooth(Activado/desactivado)
            if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                //Obtengo el parametro, aplicando un Bundle, que me indica el estado del Bluetooth
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);

                if (state == BluetoothAdapter.STATE_ON) {
                    Log.i("BLUETOOTH_BROADCAST", "ACTIVADO");
                    showEnabled();
                } else {
                    Log.i("BLUETOOTH_BROADCAST", "DESACTIVADO");
                    showDisabled();
                }
            }
            //Si se inicio la busqueda de dispositivos Bluetooth
            else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                //Creo la lista donde voy a mostrar los dispositivos encontrados
                mDeviceList = new ArrayList<BluetoothDevice>();
                Log.i("BLUETOOTH_BROADCAST", "BUSQUEDA");
            }
            //si se encontro un dispositivo bluethoot
            else if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                //Se lo agregan sus datos a una lista de dispositivos encontrados
                device = (BluetoothDevice) intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                mDeviceList.add(device);
                Log.i("BLUETOOTH_BROADCAST", "Dispositivo Encontrado:" + device.getName());
            }
        }
    };

    private void showEnabled() {
        Log.i("BLUETOOTH","Habilitado");
        txtStatusBluetooth.setText("Bluetooth Habilitado");
        txtStatusBluetooth.setTextColor(Color.WHITE);
        buttonActivate.setEnabled(false);
        buttonSearchDevices.setEnabled(true);
    }

    private void showDisabled() {
        Log.i("BLUETOOTH","Deshabilitado");
        txtStatusBluetooth.setText("Bluetooth Deshabilitado");
        txtStatusBluetooth.setTextColor(Color.RED);
        buttonActivate.setEnabled(true);
        buttonSearchDevices.setEnabled(false);
    }

    private void showUnsupported() {
        Log.i("BLUETOOTH","No soportado");
        txtStatusBluetooth.setText("Bluetooth Deshabilitado");
        txtStatusBluetooth.setTextColor(Color.RED);
        buttonActivate.setEnabled(false);
        buttonSearchDevices.setEnabled(false);
    }
}