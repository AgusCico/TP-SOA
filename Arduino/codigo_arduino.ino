#include <PWMServo.h>

#include <SoftwareSerial.h>

#define BT1LIBRE 'a'
#define BT1OCUPADO 'c'
#define BT2LIBRE 'd'
#define BT2OCUPADO 'e'
#define ABRIRBARRERA 'n'
#define CERRARBARRERA 'm'

#define TIMEOUT_BARRERA_MILIS 5000
#define TIEMPO_MAXIMO_ECHO 400 // Tiempo maximo que puede durar el ECHO

#define BARRERA_BAJA 90
#define BARRERA_ALTA 0

#define LED_HAY_AUTO_ESTACION1 16
#define LED_HAY_AUTO_ESTACION2 15
#define LED_LUZ_EXTERIOR 14

#define PIN_FOTORRESISTENCIA A5

#define TRIGGER_SENSOR_ESTACION1 12
#define TRIGGER_SENSOR_ESTACION2 3
#define TRIGGER_SENSOR_BARRERA 4

#define ECHO_SENSOR_ESTACION1 11
#define ECHO_SENSOR_ESTACION2 9
#define ECHO_SENSOR_BARRERA 6

#define CANTIDAD_LUZ_MINIMA 300
#define BOTON_PULSAR 2
//El Servo se encuentra en el pin 10 (SERVO_PIN_B )

#define BARRERA_BAJA 90
#define BARRERA_ALTA 0

#define ESTADO_INICIAL 1
#define ESTADO_ESPERANDO_AUTO 2
#define ESTADO_ESTACIONAMIENTO_OCUPADO 3
#define ESTADO_AUTO_SALIENDO 4
#define ESTADO_ATIENDE_AUTO 5
#define ESTADO_INGRESO_AUTO 6

#define EVENTO_NO_DETECTA_LUZ_EXTERIOR 10
#define EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO 11
#define EVENTO_DETECTA_PULSADOR 12
#define EVENTO_DETECTA_AUTO 13
#define EVENTO_TIMEOUT_BARRERA 14
#define EVENTO_TIMEOUT_SLOT_ESTACIONAMIENTO 15
#define EVENTO_NO_HAY_LUGAR 16
#define EVENTO_HAY_LUGAR 17

#define EVENTO_DETECTA_LUZ_EXTERIOR 18
#define EVENTO_DETECTA_AUTO_ESTACIONADO 19

#define CANTIDAD_SLOT_DISPONIBLE 2

#define BT_ABRIR_BARRERA 98 // b

#define BT_PIN_RX 8
#define BT_PIN_TX 7

PWMServo servo_9;

char btstring = "";
int estado_actual;
int evento_actual;

int hayAuto = 0;
int boton; // // se declara la variable boton

int slots_disponibles = CANTIDAD_SLOT_DISPONIBLE;
bool slot1_libre = true;
bool slot2_libre = true;
unsigned long tiempo_actual;
unsigned long tiempo_anterior;
bool timerBarrera = false;
bool detectoLuzExterior = true;
int slot1_led = HIGH;
int slot2_led = HIGH;

SoftwareSerial BTserial(BT_PIN_RX, BT_PIN_TX); // RX TX

int distanciaAuto(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);

  // limpio el TRIGGER
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // pongo HIGH el trigger por 10 microsegundos
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);

  // Leo la señal ECHO y retorno el tiempo del sondio
  return pulseIn(echoPin, HIGH);
}

// Función
bool detectarPresencia(int trigger, int echo)
{
  int hayAuto = distanciaAuto(trigger, echo);
  if (hayAuto >= TIEMPO_MAXIMO_ECHO)
  {
    return false;
  }
  else
  {
    return true;
  }
}

// FUNCION PARA ENVIO DE DATOS DEL SENSOR HACIA BLUETOOTH
void informarSlot1PorBT()
{
  Serial.println(btstring);
  BTserial.print(btstring);
}

// Función para verificar si un auto ocupa el slot
bool obtenerEntraAuto()
{
  bool entraAuto = false;
  bool detectaPresencia = false;

  if (slot1_libre)
  {
    detectaPresencia = detectarPresencia(TRIGGER_SENSOR_ESTACION1, ECHO_SENSOR_ESTACION1);
    if (detectaPresencia)
    {
      slot1_libre = false;
      entraAuto = true;
      slot1_led = LOW;
      btstring = BT1OCUPADO;
    }
  }

  if (slot2_libre)
  {
    detectaPresencia = detectarPresencia(TRIGGER_SENSOR_ESTACION2, ECHO_SENSOR_ESTACION2);
    if (detectaPresencia)
    {
      slot2_libre = false;
      entraAuto = true;
      slot2_led = LOW;
      btstring = BT2OCUPADO;
    }
  }

  return entraAuto;
}

// Función para verificar si un auto libera slot
bool obtenerSaleAuto()
{
  bool saleAuto = false;
  bool detectaPresencia = false;

  if (!slot1_libre)
  {
    detectaPresencia = detectarPresencia(TRIGGER_SENSOR_ESTACION1, ECHO_SENSOR_ESTACION1);
    if (!detectaPresencia)
    {
      slot1_libre = true;
      saleAuto = true;
      slot1_led = HIGH;
      btstring = BT1LIBRE;
    }
  }

  if (!slot2_libre)
  {
    detectaPresencia = detectarPresencia(TRIGGER_SENSOR_ESTACION2, ECHO_SENSOR_ESTACION2);
    if (!detectaPresencia)
    {
      slot2_libre = true;
      saleAuto = true;
      slot2_led = HIGH;
      btstring = BT2LIBRE;
    }
  }

  return saleAuto;
}

// Función para dar comienzo al timer de barrera
bool obtenerTimerBarrera()
{
  // Toma el tiempo en el que se abrió la barrera.
  tiempo_anterior = millis();
  timerBarrera = true;
  return timerBarrera;
}

// Función que valua cantidad de luz captada por sensor fotoresistencia
bool detectaLuzExterior()
{
  bool hayLuzSuficiente = false;
  int entradaLuz;
  entradaLuz = analogRead(PIN_FOTORRESISTENCIA);
  if (entradaLuz > CANTIDAD_LUZ_MINIMA)
  {
    hayLuzSuficiente = true;
  }
  return hayLuzSuficiente;
}

// Función para abrir barrera
void abrirBarrera()
{
  BTserial.print(ABRIRBARRERA);
  servo_9.write(BARRERA_ALTA);
}

// Función para cerrar barrera
void cerrarBarrera()
{
  BTserial.print(CERRARBARRERA);
  servo_9.write(BARRERA_BAJA);
}

// Función que enciende la luz del estacionamiento
void encenderLuzNoche()
{
  digitalWrite(LED_LUZ_EXTERIOR, HIGH);
}

// Función que apaga la luz del estacionamiento
void apagarLuzNoche()
{
  digitalWrite(LED_LUZ_EXTERIOR, LOW);
}

// Función que se encarga de actualizar los leds de indicadores de estacionamiento
void actualizar_leds_estacionamiento()
{
  digitalWrite(LED_HAY_AUTO_ESTACION1, slot1_led);
  digitalWrite(LED_HAY_AUTO_ESTACION2, slot2_led);
}

// Función que se encarga de evaluar los sensores del embedido y generar eventos del sistema
void tomar_evento()
{
  bool saleAuto;
  bool entraAuto;
  int accionBT = 0;

  if (BTserial.available())
  {
    accionBT = BTserial.read();
    Serial.print("Accion BT: ");
    Serial.println(accionBT); // muestro por output

    if (accionBT == BT_ABRIR_BARRERA)
    {
      Serial.println("BT_ABRIR_BARRERA");
    }
  }
  // SENSOR DISTACIA ESTACIONAMIENTO: Validamos si un auto está saliendo

  saleAuto = obtenerSaleAuto();
  if (saleAuto)
  {
    evento_actual = EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO;
  }
  entraAuto = obtenerEntraAuto();
  if (entraAuto)
  {
    evento_actual = EVENTO_HAY_LUGAR;
  }
  if (!entraAuto && !saleAuto)
  {
    evento_actual = EVENTO_TIMEOUT_SLOT_ESTACIONAMIENTO;
  }
  if (!slot1_libre && !slot2_libre)
  {
    evento_actual = EVENTO_NO_HAY_LUGAR;
  }

  if (detectoLuzExterior != detectaLuzExterior())
  {
    detectoLuzExterior = detectaLuzExterior();
    if (detectoLuzExterior)
    {
      evento_actual = EVENTO_DETECTA_LUZ_EXTERIOR;
    }
    else
    {
      evento_actual = EVENTO_NO_DETECTA_LUZ_EXTERIOR;
    }
  }

  // SENSOR DITANCIA BARRERA: Validamos si hay presencia en la barrera
  if (timerBarrera)
  {
    // Toma el tiempo actual.
    tiempo_actual = millis();
    bool prescenciaBarrera = detectarPresencia(TRIGGER_SENSOR_BARRERA, ECHO_SENSOR_BARRERA);

    if (prescenciaBarrera)
    {
      evento_actual = EVENTO_DETECTA_AUTO;
      timerBarrera = false;
    }
    else
    {
      if ((tiempo_actual - tiempo_anterior) > TIMEOUT_BARRERA_MILIS)
      {
        evento_actual = EVENTO_TIMEOUT_BARRERA;
        timerBarrera = false;
      }
    }
  }

  // SENSOR PULSADOR: Analizamos pulsador
  boton = digitalRead(BOTON_PULSAR); // se asigna a la variable “boton” el valor del pin 12
  if (boton || accionBT == BT_ABRIR_BARRERA)
  {
    evento_actual = EVENTO_DETECTA_PULSADOR;
    Serial.println("Se presiona pulsador, Evento EVENTO_DETECTA_PULSADOR");
  }
}

// Función para administrar maquina de estados
void fsm()
{
  tomar_evento();


  switch (estado_actual)
  {
  case ESTADO_INICIAL:
    switch (evento_actual)
    {
    case EVENTO_NO_DETECTA_LUZ_EXTERIOR:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_INICIAL...");
      Serial.println("Evento EVENTO_NO_DETECTA_LUZ_EXTERIOR...");
      Serial.println("-----------------------------------------------------");
      encenderLuzNoche();
      estado_actual = ESTADO_ESPERANDO_AUTO;
      break;

    case EVENTO_DETECTA_LUZ_EXTERIOR:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_INICIAL...");
      Serial.println("Evento EVENTO_DETECTA_LUZ_EXTERIOR...");
      Serial.println("-----------------------------------------------------");
      apagarLuzNoche();
      estado_actual = ESTADO_ESPERANDO_AUTO;
      break;
    }
    break;

  case ESTADO_ESPERANDO_AUTO:
    switch (evento_actual)
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
      actualizar_leds_estacionamiento();
      estado_actual = ESTADO_AUTO_SALIENDO;
      break;
    case EVENTO_NO_DETECTA_LUZ_EXTERIOR:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_ESPERANDO_AUTO...");
      Serial.println("Evento EVENTO_NO_DETECTA_LUZ_EXTERIOR...");
      Serial.println("-----------------------------------------------------");
      encenderLuzNoche();
      estado_actual = ESTADO_ESPERANDO_AUTO;
      break;

    case EVENTO_DETECTA_LUZ_EXTERIOR:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_ESPERANDO_AUTO...");
      Serial.println("Evento EVENTO_DETECTA_LUZ_EXTERIOR...");
      Serial.println("-----------------------------------------------------");
      apagarLuzNoche();
      estado_actual = ESTADO_ESPERANDO_AUTO;
      break;

    default:
      break;
    }
    break;

  case ESTADO_ATIENDE_AUTO:

    switch (evento_actual)
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

    switch (evento_actual)
    {
    case EVENTO_HAY_LUGAR:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_INGRESO_AUTO...");
      Serial.println("Evento EVENTO_HAY_LUGAR...");
      Serial.println("-----------------------------------------------------");
      actualizar_leds_estacionamiento();
      informarSlot1PorBT();
      estado_actual = ESTADO_ESPERANDO_AUTO;

      break;

    case EVENTO_NO_HAY_LUGAR:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_INGRESO_AUTO...");
      Serial.println("Evento EVENTO_NO_HAY_LUGAR...");
      Serial.println("-----------------------------------------------------");
      actualizar_leds_estacionamiento();
      informarSlot1PorBT();
      estado_actual = ESTADO_ESTACIONAMIENTO_OCUPADO;
      break;

    default:
      break;
    }
    break;

  case ESTADO_ESTACIONAMIENTO_OCUPADO:
    switch (evento_actual)
    {
    case EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_ESTACIONAMIENTO_OCUPADO...");
      Serial.println("Evento EVENTO_DEJA_DETECTAR_AUTO_ESTACIONADO...");
      Serial.println("-----------------------------------------------------");
      actualizar_leds_estacionamiento();
      estado_actual = ESTADO_AUTO_SALIENDO;
      break;
    case EVENTO_NO_DETECTA_LUZ_EXTERIOR:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_ESTACIONAMIENTO_OCUPADO...");
      Serial.println("Evento EVENTO_NO_DETECTA_LUZ_EXTERIOR...");
      Serial.println("-----------------------------------------------------");
      encenderLuzNoche();
      estado_actual = ESTADO_ESTACIONAMIENTO_OCUPADO;
      break;

    case EVENTO_DETECTA_LUZ_EXTERIOR:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_ESTACIONAMIENTO_OCUPADO...");
      Serial.println("Evento EVENTO_DETECTA_LUZ_EXTERIOR...");
      Serial.println("-----------------------------------------------------");
      apagarLuzNoche();
      estado_actual = ESTADO_ESTACIONAMIENTO_OCUPADO;
      break;

    default:
      break;
    }
    break;

  case ESTADO_AUTO_SALIENDO:

    switch (evento_actual)
    {
    case EVENTO_TIMEOUT_SLOT_ESTACIONAMIENTO:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_AUTO_SALIENDO...");
      Serial.println("Evento EVENTO_TIMEOUT_SLOT_ESTACIONAMIENTO...");
      Serial.println("-----------------------------------------------------");
      estado_actual = ESTADO_ESPERANDO_AUTO;
       informarSlot1PorBT();
      actualizar_leds_estacionamiento();
      break;

    case EVENTO_HAY_LUGAR:
      Serial.println("-----------------------------------------------------");
      Serial.println("Estado ESTADO_AUTO_SALIENDO...");
      Serial.println("Evento EVENTO_DETECTA_AUTO...");
      Serial.println("-----------------------------------------------------");
      actualizar_leds_estacionamiento();
      informarSlot1PorBT();
      estado_actual = ESTADO_ESPERANDO_AUTO;
      break;

    default:
      break;
    }

    break;

  default:
    break;
  }
}

// Función de setup inicial
void setup()
{
  Serial.begin(9600);
  BTserial.begin(9600);
  pinMode(PIN_FOTORRESISTENCIA, INPUT);
  pinMode(LED_LUZ_EXTERIOR, OUTPUT);
  pinMode(LED_HAY_AUTO_ESTACION1, OUTPUT);
  pinMode(LED_HAY_AUTO_ESTACION2, OUTPUT);
  pinMode(BOTON_PULSAR, INPUT); // se declara el pin 12 como entrada
  servo_9.attach(SERVO_PIN_B);
  servo_9.write(BARRERA_BAJA);

  // Encendemos los Leds de indicadores de estacionamiento (Estacionamiento libre)
  digitalWrite(LED_HAY_AUTO_ESTACION1, HIGH);
  digitalWrite(LED_HAY_AUTO_ESTACION2, HIGH);

  detectoLuzExterior = detectaLuzExterior();

  // Seteamos estado inicial
  estado_actual = ESTADO_INICIAL;
}

// Hilo principal del embebido
void loop()
{
  fsm();
}