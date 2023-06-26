package com.m5.smartparking.service;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class SmartParkingBluetooth extends Thread {

    private static final UUID BTMODULEUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private BluetoothAdapter btAdapter;
    private BluetoothDevice btDevice;
    private BluetoothSocket btSocket;
    private InputStream mmInStream;
    private OutputStream mmOutStream;

    private Handler handlerBluetoothIn;
    private int handlerState = 0;
    private String deviceName;
    private boolean finalizado = false;

    private static final int BUFFER_SIZE = 256;

    @SuppressLint("MissingPermission")
    public SmartParkingBluetooth(String deviceAddress, String deviceName){
        this.deviceName = deviceName;

        btAdapter = BluetoothAdapter.getDefaultAdapter();
        btDevice = btAdapter.getRemoteDevice(deviceAddress);

        try {
            btSocket = btDevice.createRfcommSocketToServiceRecord(BTMODULEUUID);
        }
        catch (IOException e) {
            Log.i("SOCKET","fallo dispositivo "+ deviceName);
        }

        try {
            btSocket.connect();
            Log.i("SOCKET","conectado "+ deviceName);
        }
        catch (IOException e) {
            Log.i("SOCKET","fallo " + deviceName + " " + e.toString());
        }

        InputStream tmpIn = null;
        OutputStream tmpOut = null;

        try {
            tmpIn = btSocket.getInputStream();
            tmpOut = btSocket.getOutputStream();
        } catch (IOException e) { }

        mmInStream = tmpIn;
        mmOutStream = tmpOut;
    }

    public void setHandler(Handler handlerBluetoothIn) {
        this.handlerBluetoothIn = handlerBluetoothIn;
    }

    public void run() {
        Log.i("THREAD", "Iniciado " + deviceName);

        final int zero = 0;

        byte[] buffer = new byte[BUFFER_SIZE];
        int bytes;

        //el hilo secundario se queda esperando mensajes del HC05
        while (true) {
            try {
                //se leen los datos del Bluethoot
                bytes = this.mmInStream.read(buffer);
                String readMessage = new String(buffer, zero, bytes);
                System.out.println(readMessage);
                //se muestran en el layout de la activity, utilizando el handler del hilo
                // principal antes mencionado

                this.handlerBluetoothIn.obtainMessage(this.handlerState, bytes, -1, readMessage).sendToTarget();
            } catch (IOException e) {
                break;
            }
        }
    }

    public void write(String input) {
        byte[] msgBuffer = input.getBytes();
        try {
            this.mmOutStream.write(msgBuffer);
        } catch (IOException e) {
            Log.i("Conection Failure", e.toString());
        }
    }
}
