#include <Servo.h>

#define TIEMPO_MAXIMO 19350 //Tiempo maximo que puede durar el ECHO
#define	LED_HAY_AUTO_ESTACION1 7
#define LED_HAY_AUTO_ESTACION2 5
#define LED_LUZ_EXTERIOR 4
// #define LED_BOTON 2

#define TRIGGER_SENSOR_ESTACION1 12
#define TRIGGER_SENSOR_ESTACION2 10
#define TRIGGER_SENSOR_BARRERA 8

#define ECHO_SENSOR_ESTACION1 11
#define ECHO_SENSOR_ESTACION2 9
#define ECHO_SENSOR_BARRERA 6

#define CANTIDAD_LUZ_MINIMA 300
#define BOTON_PULSAR 2
#define SEÑAL_SERVO 3

#define ESTADO_INICIAL 1
#define ESTADO_ESPERANDO_AUTO 2
#define ESTADO_ESTACIONAMIENTO_OCUPADO 3
#define ESTADO_AUTO_SALIENDO 4
#define ESTADO_ATIENDE_AUTO 5
#define ESTADO_INGRESO_AUTO 6

// #define EVENTO_DETECTA_LUZ_EXTERIOR 
#define EVENTO_NO_DETECTA_LUZ_EXTERIOR 10
#define EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO 11
#define EVENTO_DETECTA_PULSADOR 12
#define EVENTO_DETECTA_AUTO 13
#define EVENTO_TIMEOUT_BARRERA 14
#define EVENTO_TIMEOUT_SLOT_ESTACIONAMIENTO 15
#define EVENTO_NO_HAY_LUGAR 16
#define EVENTO_HAY_LUGAR 17

// #define constantes de trabajo
#define TIEMPO_MAX_MILIS 900

//int pos = 0;
Servo servo_9;
//int entradaLuz;

int estado_actual;
int evento_actual;

int hayAuto=0;
int boton;  // // se declara la variable boton

int slots_disponibles = CANTIDAD_SLOT_DISPONIBLE;
bool slot1_libre = true;
bool slot2_libre = true;
unsigned long tiempo_actual;  
unsigned long tiempo_anterior;
bool timerBarrera = false;


int distanciaAuto(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT); 
  
  // limpio el TRIGGER
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  
  //pongo HIGH el trigger por 10 microsegundos
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  
  //Leo la señal ECHO y retorno el tiempo del sondio
  return pulseIn(echoPin, HIGH);
}

void 

void abrirBarrera()
{
  servo_9.write(90);
  //delay(1000);
}

void cerrarBarrera()
{
  servo_9.write(0);
  //delay(1000);
}

bool detectaPresencia(int trigger, int echo)
{
  bool detectaPresencia;
  hayAuto = distanciaAuto(trigger,echo);
  
  if(hayAuto >= TIEMPO_MAXIMO)
  {
    detectaPresencia = false;
  }
  else
  {
    detectaPresencia = true;
  }
  return detectaPresencia;
}

bool saleAuto()
{
  bool saleAuto = false;
  bool detectaPresencia = false;


  if(!slot1_libre)
  {
    detectaPresencia = detectaPresencia(TRIGGER_SENSOR_ESTACION1, ECHO_SENSOR_ESTACION1);
    if(!detectaPresencia)
    {
      slot1_libre = true;
      saleAuto = true;
    }
  }

    if(!slot2_libre)
  {
    detectaPresencia = detectaPresencia(TRIGGER_SENSOR_ESTACION2, ECHO_SENSOR_ESTACION2);
    if(!detectaPresencia)
    {
      slot2_libre = true;
      saleAuto = true;
    }
  }

  return saleAuto;
}

void encenderLuzNoche()
{
  int entradaLuz;
  entradaLuz=analogRead(A5);

  if(entradaLuz < CANTIDAD_LUZ)
  {
    digitalWrite(LED_LUZ_EXTERIOR,HIGH);
  }
  else
  {
    digitalWrite(LED_LUZ_EXTERIOR,LOW);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(A5, INPUT);
  pinMode(LED_LUZ_EXTERIOR, OUTPUT);
  pinMode(LED_HAY_AUTO_ESTACION1, OUTPUT);
  pinMode(LED_HAY_AUTO_ESTACION2, OUTPUT);
  // pinMode(LED_BOTON, OUTPUT); // se declara el pin 2 como salida
  pinMode(BOTON_PULSAR,INPUT); // se declara el pin 12 como entrada
  // pinMode(LED_BUILTIN, OUTPUT);
  servo_9.attach(SEÑAL_SERVO);
  servo_9.write(90);
  //delay(5000);
  Serial.println("listo");
}

bool timerBarrera()
{
  // Toma el tiempo en el que se abrió la barrera.
  tiempo_anterior = millis();
  timerBarrera = true;
  return timerBarrera;
}

void tomar_evento()
{
  bool saleAuto;

  //Analizamos pulsador
  boton=digitalRead(BOTON_PULSAR);  //se asigna a la variable “boton” el valor del pin 12
  if(boton)
  {
    evento_actual = EVENTO_DETECTA_PULSADOR;
    Serial.println("Se presiona pulsador, Evento EVENTO_DETECTA_PULSADOR");
  }  

  //Validamos si un auto está saliendo 
  saleAuto = saleAuto();
  if(saleAuto)
  {
    evento_actual = EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO;
  }

  //Validamos si hay presencia en la barrera
  if(timerBarrera)
  {
    // Toma el tiempo actual.
    tiempo_actual = millis();
    detectaPresencia(TRIGGER_SENSOR_BARRERA, ECHO_SENSOR_BARRERA);
    if((tiempo_actual-tiempo_anterior) > TIEMPO_MAX_MILIS )
    {
      evento_actual = EVENTO_TIMEOUT_BARRERA;
    }
  }


}

void actualizar_leds()
{
  int slot1_led = digitalRead(LED_HAY_AUTO_ESTACION1);
  if(slot1_libre && slot1_led == LOW)
  {
    digitalWrite(LED_HAY_AUTO_ESTACION1, HIGH);  
  }

  int slot2_led = digitalRead(LED_HAY_AUTO_ESTACION2);
  if(slot2_libre && slot2_led == LOW)
  {
    digitalWrite(LED_HAY_AUTO_ESTACION2, HIGH);  
  }
}

void fsm()
{

  tomar_evento();

  switch(estado_actual)
  {
    case ESTADO_INICIAL:
      switch(evento_actual)
      {
        case :

          break;

        default:
          break;
      }
      break;

    case ESTADO_ESPERANDO_AUTO:

      switch(evento_actual)
      {
        case EVENTO_DETECTA_PULSADOR:
				  Serial.println("-----------------------------------------------------");
				  Serial.println("Estado ESTADO_ESPERANDO_AUTO...");
				  Serial.println("Evento EVENTO_DETECTA_PULSADOR...");
				  Serial.println("-----------------------------------------------------");

          abrirBarrera();
          timerBarrera();
          estado_actual = ESTADO_ATIENDE_AUTO;
          break;

        case EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO:
				  Serial.println("-----------------------------------------------------");
				  Serial.println("Estado ESTADO_ESPERANDO_AUTO...");
				  Serial.println("Evento EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO...");
				  Serial.println("-----------------------------------------------------");
          actualizar_leds();
          estado_actual = ESTADO_AUTO_SALIENDO;
          break;

        default:
          break;
      }
      break;
    
    case ESTADO_ATIENDE_AUTO:

      switch(evento_actual)
      {
        case EVENTO_DETECTA_AUTO:

          break;

        case EVENTO_TIMEOUT_BARRERA:

          break;

        default:
          break;
      }

      break;


    case ESTADO_INGRESO_AUTO:
      
      switch(evento_actual)
      {
        case EVENTO_HAY_LUGAR:

          break;
        
        case EVENTO_NO_HAY_LUGAR:

          break;

        case EVENTO_NO_DETECTA_LUZ_EXTERIOR:

          break;

        default:
          break;
      }
      break;

    
    case ESTADO_ESTACIONAMIENTO_OCUPADO:
      switch(evento_actual)
      {
        case EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO:

          break;

        default:
          break;
      }
      break;


    case ESTADO_AUTO_SALIENDO:

      switch(evento_actual)
      {
        case EVENTO_TIMEOUT_SLOT_ESTACIONAMIENTO:

          break;

        case EVENTO_DETECTA_AUTO:

          break;

        case EVENTO_NO_DETECTA_LUZ_EXTERIOR:

          break;

        default:
          break;
      }

      break;

    

    default:
      break;

  }

}

void loop()
{
  fsm();
  //Serial.println(entradaLuz);
  boton=digitalRead(BOTON_PULSAR);  //se asigna a la variable “boton” el valor del pin 12
  // digitalWrite(LED_BOTON,boton);  
  

  abrirBarrera();
  detectaPresencia(TRIGGER_SENSOR_ESTACION1, ECHO_SENSOR_ESTACION1, LED_HAY_AUTO_ESTACION1);
  detectaPresencia(TRIGGER_SENSOR_ESTACION2, ECHO_SENSOR_ESTACION2, LED_HAY_AUTO_ESTACION2);
  encenderLuzNoche();
  
}