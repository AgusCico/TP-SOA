package com.m5.smartparking;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class Login extends AppCompatActivity 
{

    private TextView user;
    private TextView password;
    private Button login;

    private final String USER = "admin";
    private final String PASSWORD = "1234";

    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        user = (TextView) findViewById(R.id.txtUser);
        password = (TextView) findViewById(R.id.txtPassword);
        login = (Button) findViewById(R.id.buttonLogin);

        login.setOnClickListener(new View.OnClickListener() 
        {
            @Override
            public void onClick(View view) 
            {
                if(user.getText().toString().equals(USER) && password.getText().toString().equals(PASSWORD)) 
                {
                    Intent intent = new Intent(Login.this, MainActivity.class);
                    startActivity(intent);
                } 
                else 
                {
                    Toast.makeText(getApplicationContext(), "Datos erroneos. Intente nuevamente", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }
}