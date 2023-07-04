package com.m5.smartparking;


import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;


public class DispositivoConectado extends Activity implements SensorEventListener, CompoundButton.OnCheckedChangeListener 
{


    private final static float ACC = 13;
    private CheckBox switchButton;
    private Button openBarrier;
    private TextView textBarrera;
    private TextView txtSpot1;
    private TextView txtSpot2;

    private SensorManager sensor;

    //-------------------------------------------
    Handler bluetoothIn;
    final int handlerState = 0;
    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private StringBuilder DataStringIN = new StringBuilder();
    private ConnectedThread MyConexionBT;
    // Identificador unico de servicio - SPP UUID
    private static final UUID BTMODULEUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    public static String EXTRA_DEVICE_ADDRESS = "device_address";
    // String para la direccion MAC
    private static String address = null;
    //-------------------------------------------

    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dispositivo_conectado);

        sensor = (SensorManager) getSystemService(SENSOR_SERVICE);

        textBarrera = findViewById(R.id.estadoBarrera);
        txtSpot1 = findViewById(R.id.spot1);
        txtSpot2 = findViewById(R.id.spot2);
        switchButton = findViewById(R.id.checkBoxShake);
        switchButton.setOnCheckedChangeListener(this);
        openBarrier = (Button) findViewById(R.id.abrirBarrera);

        //-----------------------------------------
        bluetoothIn = new Handler() 
        {
            public void handleMessage(android.os.Message msg)
             {
                if (msg.what == handlerState) 
                {
                    char MyCaracter = (char) msg.obj;

                    String str = String.valueOf(MyCaracter);

                    Log.i("VALOR RECIBIDO", str);

                    switch (MyCaracter) 
                    {
                        case 'a':
                            Log.i("ENTRA", "1");
                            txtSpot1.setText("LIBRE");
                            break;
                        case 'c':
                            Log.i("ENTRA", "2");
                            txtSpot1.setText("OCUPADO");
                            break;
                        case 'd':
                            Log.i("ENTRA", "3");
                            txtSpot2.setText("LIBRE");
                            break;
                        case 'e':
                            Log.i("ENTRA", "4");
                            txtSpot2.setText("OCUPADO");
                            break;
                        case 'm':
                            textBarrera.setText("CERRADA");
                            break;
                        case 'n':
                            textBarrera.setText("ABIERTA");
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        btAdapter = BluetoothAdapter.getDefaultAdapter(); // get Bluetooth adapter
        VerificarEstadoBT();

        openBarrier.setOnClickListener(new View.OnClickListener() 
        {
            @Override
            public void onClick(View view) 
            {
                MyConexionBT.write("b");
            }
        });

    }


    @Override
    protected void onResume()
    {
        super.onResume();

        if (switchButton.isChecked())
        {
            registerSenser();
        }

        Intent intent = getIntent();
        address = intent.getStringExtra(DispositivosVinculados.EXTRA_DEVICE_ADDRESS);
        Log.i("ADDRESS", address);
        //Setea la direccion MAC
        BluetoothDevice device = btAdapter.getRemoteDevice(address);

        try 
        {
            btSocket = createBluetoothSocket(device);
        }
        catch (IOException e) 
        {
            Toast.makeText(getBaseContext(), "La creacción del Socket fallo", Toast.LENGTH_LONG).show();
        }
        // Establece la conexión con el socket Bluetooth.
        try 
        {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) 
            {
                btSocket.connect();
                Toast.makeText(getBaseContext(), "CONEXION EXITOSA", Toast.LENGTH_SHORT).show();
            }

        } 
        catch (IOException e) 
        {
            try 
            {
                btSocket.close();
            } 
            catch (IOException e2) 
            {
            }
        }
        MyConexionBT = new ConnectedThread(btSocket);
        MyConexionBT.start();

    }

    @Override
    protected void onStop()
    {
        super.onStop();
        unregisterSenser();
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        if (switchButton.isChecked())
        {
            unregisterSenser();
        }
        try { // Cuando se sale de la aplicación esta parte permite que no se deje abierto el socket
            btSocket.close();
        }
         catch (IOException e2) {
        }
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
                MyConexionBT.write("b");
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

    //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    private BluetoothSocket createBluetoothSocket(BluetoothDevice device) throws IOException 
    {
        return device.createRfcommSocketToServiceRecord(BTMODULEUUID);
    }

    //Comprueba que el dispositivo Bluetooth
    //está disponible y solicita que se active si está desactivado
    private void VerificarEstadoBT() 
    {

        if (btAdapter == null) 
        {
            Toast.makeText(getBaseContext(), "El dispositivo no soporta bluetooth", Toast.LENGTH_LONG).show();
        } 
        else
        {
            if (btAdapter.isEnabled()) 
            {
                Log.i("Estado Bluetooth", "Activado");
            } 
            else 
            {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) 
                {
                    startActivityForResult(enableBtIntent, 1);
                }

            }
        }
    }

    //Crea la clase que permite crear el evento de conexion
    private class ConnectedThread extends Thread
    {
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public ConnectedThread(BluetoothSocket socket)
        {
            InputStream tmpIn = null;
            OutputStream tmpOut = null;
            try
            {
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            }
            catch (IOException e) { }
            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run()
        {
            byte[] byte_in = new byte[1];
            // Se mantiene en modo escucha para determinar el ingreso de datos
            while (true) 
            {
                try 
                {
                    mmInStream.read(byte_in);
                    char ch = (char) byte_in[0];
                    bluetoothIn.obtainMessage(handlerState, ch).sendToTarget();
                } 
                catch (IOException e) 
                {
                    break;
                }
            }
        }

        //Envio de trama
        public void write(String input)
        {
            try 
            {
                mmOutStream.write(input.getBytes());
            }
            catch (IOException e)
            {
                //si no es posible enviar datos se cierra la conexión
                Toast.makeText(getBaseContext(), "La Conexión fallo", Toast.LENGTH_LONG).show();
                finish();
            }
        }
    }
}