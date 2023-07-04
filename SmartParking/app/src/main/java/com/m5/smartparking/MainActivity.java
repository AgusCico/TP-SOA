package com.m5.smartparking;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ProgressDialog;
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
import android.util.LogPrinter;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class MainActivity extends Activity {

    private TextView txtStatusBluetooth;
    private Button buttonActivate;
    private Button buttonSearchDevices;
    private BluetoothAdapter mBluetoothAdapter;
    private ProgressDialog mProgressDlg;
    public static final int MULTIPLE_PERMISSIONS = 10;

    String[] permissions = new String[]{
            Manifest.permission.BLUETOOTH,
            Manifest.permission.BLUETOOTH_ADMIN,
            Manifest.permission.BLUETOOTH_CONNECT,
            Manifest.permission.ACCESS_COARSE_LOCATION};

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
                Intent intent = new Intent(MainActivity.this, DispositivosVinculados.class);
                startActivity(intent);

            }
        });

        if(checkPermissions()) {
            Log.i("PERMISSIONS","TRUE");
            enableComponent();
        }
    }

    protected void enableComponent()
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
                    Log.i("BLUETOOTH", "ACTIVADO");
                    showEnabled();
                } else {
                    Log.i("BLUETOOTH", "DESACTIVADO");
                    showDisabled();
                }
            }
        }
    };

    private void showEnabled() {
        Log.i("BLUETOOTH","Habilitado");
        txtStatusBluetooth.setText("Bluetooth Habilitado");
        txtStatusBluetooth.setTextColor(Color.parseColor("#323232"));
        buttonActivate.setEnabled(false);
        buttonActivate.setBackgroundColor(Color.GRAY);
        buttonSearchDevices.setEnabled(true);
    }

    private void showDisabled() {
        Log.i("BLUETOOTH","Deshabilitado");
        txtStatusBluetooth.setText("Bluetooth Deshabilitado");
        txtStatusBluetooth.setTextColor(Color.RED);
        buttonActivate.setEnabled(true);
        buttonActivate.setBackgroundColor(Color.parseColor("#F05D22"));
        buttonSearchDevices.setEnabled(false);
    }

    private void showUnsupported() {
        Log.i("BLUETOOTH","No soportado");
        txtStatusBluetooth.setText("Bluetooth Deshabilitado");
        txtStatusBluetooth.setTextColor(Color.RED);
        buttonActivate.setEnabled(false);
        buttonSearchDevices.setEnabled(false);
    }

    //Metodo que chequea si estan habilitados los permisos
    private  boolean checkPermissions() {
        int result;
        List<String> listPermissionsNeeded = new ArrayList<>();

        //Se chequea si la version de Android es menor a la 6
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            return true;
        }

        for (String p:permissions) {
            result = ContextCompat.checkSelfPermission(this,p);
            if (result != PackageManager.PERMISSION_GRANTED) {
                listPermissionsNeeded.add(p);
            }
        }

        if (!listPermissionsNeeded.isEmpty()) {
            ActivityCompat.requestPermissions(this, listPermissionsNeeded.toArray(new String[listPermissionsNeeded.size()]), MULTIPLE_PERMISSIONS);
            return false;
        }

        return true;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case MULTIPLE_PERMISSIONS: {
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // permissions granted.
                    enableComponent(); // Now you call here what ever you want :)
                } else {
                    String perStr = "";
                    for (String per : permissions) {
                        perStr += "\n" + per;
                    }
                    // permissions list of don't granted permission
                    Toast.makeText(this, "ATENCION: La aplicacion no funcionara " +
                            "correctamente debido a la falta de Permisos", Toast.LENGTH_LONG).show();
                }
                return;
            }
        }
    }
}