#include <Servo.h>


#define TIMEOUT_BARRERA_MILIS 5000
#define TIEMPO_MAXIMO_ECHO 19000 //Tiempo maximo que puede durar el ECHO


#define BARRERA_BAJA 90
#define BARRERA_ALTA 0

#define	LED_HAY_AUTO_ESTACION1 6
#define LED_HAY_AUTO_ESTACION2 5
#define LED_LUZ_EXTERIOR 4

#define TRIGGER_SENSOR_ESTACION1 12
#define TRIGGER_SENSOR_ESTACION2 10
#define TRIGGER_SENSOR_BARRERA 8

#define ECHO_SENSOR_ESTACION1 11
#define ECHO_SENSOR_ESTACION2 9
#define ECHO_SENSOR_BARRERA 7

#define CANTIDAD_LUZ_MINIMA 300
#define BOTON_PULSAR 2
#define SENIAL_SERVO 3

#define BARRERA_BAJA 90
#define BARRERA_ALTA 0

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
#define CANTIDAD_SLOT_DISPONIBLE 2

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

void abrirBarrera()
{
  servo_9.write(BARRERA_ALTA);
  //delay(1000);
}

void cerrarBarrera()
{
  servo_9.write(BARRERA_BAJA);
  //delay(1000);
}


bool detectarPresencia(int trigger, int echo)
{
  int hayAuto = distanciaAuto(trigger,echo);
  if(hayAuto >= TIEMPO_MAXIMO_ECHO)
  {
    return false;
  }
  else
  {
  	return true;  
  }
}


bool obtenerSaleAuto()
{
  bool saleAuto = false;
  bool detectaPresencia = false;


  if(!slot1_libre)
  {
    detectaPresencia = detectarPresencia(TRIGGER_SENSOR_ESTACION1, ECHO_SENSOR_ESTACION1);
    if(!detectaPresencia)
    {
      slot1_libre = true;
      saleAuto = true;
    }
  }

    if(!slot2_libre)
  {
    detectaPresencia = detectarPresencia(TRIGGER_SENSOR_ESTACION2, ECHO_SENSOR_ESTACION2);
    if(!detectaPresencia)
    {
      slot2_libre = true;
      saleAuto = true;
    }
  }

  return saleAuto;
}
bool obtenerEntraAuto()
{
  bool entraAuto = false;
  bool detectaPresencia = false;


  if(slot1_libre)
  {
    detectaPresencia = detectarPresencia(TRIGGER_SENSOR_ESTACION1, ECHO_SENSOR_ESTACION1);
    if(detectaPresencia)
    {
      slot1_libre = false;
      entraAuto = true;
    }
  }

    if(slot2_libre)
  {
    detectaPresencia = detectarPresencia(TRIGGER_SENSOR_ESTACION2, ECHO_SENSOR_ESTACION2);
    if(detectaPresencia)
    {
      slot2_libre = false;
      entraAuto = true;
    }
  }

  return entraAuto;
}
void encenderLuzNoche()
{
  int entradaLuz;
  entradaLuz=analogRead(A5);

  if(entradaLuz < CANTIDAD_LUZ_MINIMA)
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
  servo_9.attach(SENIAL_SERVO);
  servo_9.write(BARRERA_BAJA);
  //delay(5000);
  Serial.println("listo");
  estado_actual = ESTADO_INICIAL;
}

bool obtenerTimerBarrera()
{
  // Toma el tiempo en el que se abrió la barrera.
  tiempo_anterior = millis();
  timerBarrera = true;
  return timerBarrera;
}

void tomar_evento()
{
  bool saleAuto;
  bool entraAuto;

 

  //Validamos si un auto está saliendo 
  saleAuto = obtenerSaleAuto();
  if(saleAuto)
  {
    evento_actual = EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO;
  }
  entraAuto = obtenerEntraAuto();
  if(entraAuto)
  {
    evento_actual = EVENTO_HAY_LUGAR;
  }
  if(!entraAuto && !saleAuto) 
  {
    evento_actual = EVENTO_TIMEOUT_SLOT_ESTACIONAMIENTO;
  } 
  if(!slot1_libre && !slot2_libre)
  {
    evento_actual = EVENTO_NO_HAY_LUGAR;
  }

  //Validamos si hay presencia en la barrera
  if(timerBarrera)
  {
    // Toma el tiempo actual.
    tiempo_actual = millis();
    bool prescenciaBarrera = detectarPresencia(TRIGGER_SENSOR_BARRERA, ECHO_SENSOR_BARRERA);
    
    if(prescenciaBarrera)
    {
      evento_actual = EVENTO_DETECTA_AUTO;
      timerBarrera = false;
    }
    else if((tiempo_actual-tiempo_anterior) > TIEMPO_MAX_MILIS )
    {
      evento_actual = EVENTO_TIMEOUT_BARRERA;
      timerBarrera = false;
    }
  }
  //Analizamos pulsador
  boton=digitalRead(BOTON_PULSAR);  //se asigna a la variable “boton” el valor del pin 12
  if(boton)
  {
    evento_actual = EVENTO_DETECTA_PULSADOR;
    Serial.println("Se presiona pulsador, Evento EVENTO_DETECTA_PULSADOR");
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
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_INICIAL...");
      Serial.println("Evento EVENTO_INICIAL...");
      Serial.println("-----------------------------------------------------");
      estado_actual = ESTADO_ESPERANDO_AUTO;
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
          obtenerTimerBarrera();
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
          Serial.println("-----------------------------------------------------");
          Serial.println("Estado ESTADO_ATIENDE_AUTO...");
          Serial.println("Evento EVENTO_DETECTA_AUTO...");
          Serial.println("-----------------------------------------------------"); 
          cerrarBarrera();
          estado_actual = ESTADO_INGRESO_AUTO;
          break;

        case EVENTO_TIMEOUT_BARRERA:
          Serial.println("-----------------------------------------------------");
          Serial.println("Estado ESTADO_ATIENDE_AUTO...");
          Serial.println("Evento EVENTO_TIMEOUT_BARRERA...");
          Serial.println("-----------------------------------------------------"); 
          cerrarBarrera();
          estado_actual = ESTADO_ESPERANDO_AUTO;
          break;

        default:
          break;
      }

      break;


    case ESTADO_INGRESO_AUTO:
      
      switch(evento_actual)
      {
        case EVENTO_HAY_LUGAR:
          Serial.println("-----------------------------------------------------");
          Serial.println("Estado ESTADO_INGRESO_AUTO...");
          Serial.println("Evento EVENTO_HAY_LUGAR...");
          Serial.println("-----------------------------------------------------");
          actualizar_leds();
          estado_actual = ESTADO_ESPERANDO_AUTO;

          break;
        
        case EVENTO_NO_HAY_LUGAR:
          Serial.println("-----------------------------------------------------");
          Serial.println("Estado ESTADO_INGRESO_AUTO...");
          Serial.println("Evento EVENTO_NO_HAY_LUGAR...");
          Serial.println("-----------------------------------------------------");
          estado_actual = ESTADO_ESTACIONAMIENTO_OCUPADO;
          break;

        case EVENTO_NO_DETECTA_LUZ_EXTERIOR:
          Serial.println("-----------------------------------------------------");
          Serial.println("Estado ESTADO_INGRESO_AUTO...");
          Serial.println("Evento EVENTO_NO_DETECTA_LUZ_EXTERIOR...");
          Serial.println("-----------------------------------------------------"); 
          encenderLuzNoche();
          estado_actual = ESTADO_INGRESO_AUTO;
          break;

        default:
          break;
      }
      break;

    
    case ESTADO_ESTACIONAMIENTO_OCUPADO:
      switch(evento_actual)
      {
        case EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO:
          Serial.println("-----------------------------------------------------");
          Serial.println("Estado ESTADO_ESTACIONAMIENTO_OCUPADO...");
          Serial.println("Evento EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO...");
          Serial.println("-----------------------------------------------------"); 
          actualizar_leds();
          estado_actual = ESTADO_AUTO_SALIENDO;
          break;

        default:
          break;
      }
      break;


    case ESTADO_AUTO_SALIENDO:

      switch(evento_actual)
      {
        case EVENTO_TIMEOUT_SLOT_ESTACIONAMIENTO:
          Serial.println("-----------------------------------------------------");
          Serial.println("Estado ESTADO_AUTO_SALIENDO...");
          Serial.println("Evento EVENTO_TIMEOUT_SLOT_ESTACIONAMIENTO...");
          Serial.println("-----------------------------------------------------");
          estado_actual = ESTADO_ESPERANDO_AUTO; 
          break;

        case EVENTO_HAY_LUGAR:
          Serial.println("-----------------------------------------------------");
          Serial.println("Estado ESTADO_AUTO_SALIENDO...");
          Serial.println("Evento EVENTO_DETECTA_AUTO...");
          Serial.println("-----------------------------------------------------");
          actualizar_leds();
          estado_actual = ESTADO_ESPERANDO_AUTO;
          break;

        case EVENTO_NO_DETECTA_LUZ_EXTERIOR:
          Serial.println("-----------------------------------------------------");
          Serial.println("Estado ESTADO_AUTO_SALIENDO...");
          Serial.println("Evento EVENTO_NO_DETECTA_LUZ_EXTERIOR...");
          Serial.println("-----------------------------------------------------"); 
          encenderLuzNoche();
          estado_actual = ESTADO_AUTO_SALIENDO;
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
}