#include <FreeRTOS_AVR.h>
#include "definiciones.h"

// Esta es la version en el repositorio dropbox

boolean estadoEncendido = false;
short int velocidad     = 0;
bool estadoConexion     = false;

const short int VelInicial = 0;


// Cola
QueueHandle_t cola_entrada;
QueueHandle_t cola_estados;

//Semaforos
SemaphoreHandle_t semaforo_boton;

//Prototipo de funcion task
void TareaBoton (void *pvParameters);
void TareaSalida(void *pvParameters);
void TareaSerial(void *pvParameters);

short int getTecla(void);
short int funcionSerial(void);
void printDisplay(int);



void setup() {

  setInput();
  setOutput();
  printDisplay(-1);
  Serial.begin(57600);
  DEBUG_PRINTLN ("arrancamos");
  //Colas
  cola_entrada = xQueueCreate(tamCola, tamMsg);   //Manejador de la cola delas entradas

  //Semaforos
  vSemaphoreCreateBinary(semaforo_boton );

  //Tareas

  xTaskCreate(TareaBoton,  "Tarea boton", tamPila, NULL, prioridadBoton,  NULL);
  xTaskCreate(TareaSalida, "Tarea salida", tamPila, NULL, prioridadSalida, NULL);
  xTaskCreate(TareaSerial, "Tarea serial", tamPila, NULL, prioridadSerial, NULL);

  //Inicio del planificador
  vTaskStartScheduler();

  Serial.println("Houston tenemos un problema");    //Nunca deberiamos llegar hasta aca si todo va bien
  for (;;) {

  }
}

void loop() {
  //No deberia haber nada aca, para eso estan las tareas

}


//              TAREAS

void TareaBoton (void *pvParameters) {


  while (1) {
    unsigned int botonPress[2];   //array para guardar en la cola
    botonPress[0] = getTecla();
    botonPress[1] = (int)98;

    if (botonPress[0] > 47) {
      xQueueSend(cola_entrada, &botonPress, 100);
      botonPress[0] = 0;
    }
    vTaskDelay(50);
  }
}

void TareaSerial (void *pvParameters) {


  while (1) {
    unsigned int entradaSerial[2];      //array para guardar en la cola
    entradaSerial [0] = funcionSerial();
    entradaSerial [1] = (int)115;

    if (entradaSerial[0] > 47) {   // 47 es el codigo anterior al 0 en ascii
      xQueueSend(cola_entrada, &entradaSerial, 100);
    }
    vTaskDelay(50);
  }
}

void TareaSalida (void *pvParameters) {

  unsigned int value[2] ;

  while (1) {

    //Serial.println( pcTaskName );
    if (xQueueReceive(cola_entrada, &value, portMAX_DELAY) == pdTRUE) {
      DEBUG_PRINT ((char)value[0]);
      DEBUG_PRINTLN ((char)value[1]);


      switch ((char)value[0]) {
        case 'c': estadoConexion = true;
          estadoEncendido = false;
          velocidad = VelInicial;
          togglePin(OffPin);
          printDisplay(-1);
          togglePin(StopPin);
          modoEscribir;
          Serial.println("OK");
          modoEscuchar;
          break;
        case '0': if (estadoEncendido) {
            estadoEncendido = false;
            if (estadoConexion) {
              modoEscribir;
              Serial.println('L');
              modoEscuchar;
            }
            velocidad = VelInicial;
            togglePin(OffPin);
            printDisplay(-1);
          }
          break;
        case '1': if ((!estadoEncendido)) {
            estadoEncendido = true;
            printDisplay(velocidad);
            if (estadoConexion) {
              modoEscribir;
              Serial.println('H');
              modoEscuchar;
            }
            togglePin(OnPin);
          }
          break;
        case '2': if (estadoEncendido && velocidad > 0) {
            velocidad = VelInicial;
            printDisplay(velocidad);
            if (estadoConexion) {
              modoEscribir;
              Serial.println('S');
              modoEscuchar;
            }
            togglePin(StopPin);
          }
          break;
        case '3': if (estadoEncendido && (velocidad < limiteSuperior)) {
            ++velocidad;
            printDisplay(velocidad);
            if (estadoConexion) {
              modoEscribir;
              Serial.println(velocidad);
              modoEscuchar;
            }

            togglePin(VelPinUp);
          }
          break;
        case '4': if (estadoEncendido && (velocidad > 0)) {
            --velocidad;
            printDisplay(velocidad);
            if (estadoConexion) {
              modoEscribir;
              Serial.println(velocidad);
              modoEscuchar;
            }

            togglePin(VelPinDown);
          }
          break;
        default:
          modoEscribir;
          Serial.println("ack");
          modoEscuchar;
          break;
      }
    }
  }
}


short int funcionSerial( void) {
  unsigned int entradaSerial = 0;
  modoEscuchar;
  if (Serial.available() > 0) {
    entradaSerial = Serial.read();
    //digitalWrite(LED_PIN, HIGH);
  }
  else {
    //digitalWrite(LED_PIN, LOW);
  }
  return entradaSerial;
}



short int getTecla() {
  bool estadoBotonOn       = digitalRead(botonOn);
  bool estadoBotonOff      = digitalRead(botonOff);
  bool estadoBotonVelUp    = digitalRead(botonVelUp);
  bool estadoBotonVelDown  = digitalRead(botonVelDown);
  bool estadoBotonStop     = digitalRead(botonStop);

  //if (estadoBotonOn | estadoBotonOff | estadoBotonVelUp | estadoBotonVelDown | estadoBotonStop ) {

  if (estadoBotonOff && !(estadoBotonOn | estadoBotonVelUp | estadoBotonVelDown | estadoBotonStop)) {

    while (estadoBotonOff && !(estadoBotonOn | estadoBotonVelUp | estadoBotonVelDown | estadoBotonStop))
    {
      //Esperamos a que se deje de cumplir esto para retonar el valor de la tecla
      estadoBotonOff  = digitalRead(botonOff);
    }
    return 48;    //0  valor de off
  }

  if (estadoBotonOn && ! (estadoBotonOff | estadoBotonVelUp | estadoBotonVelDown | estadoBotonStop )) {
    //tecla = 49 /*teclaVel1*/;
    while (estadoBotonOn && ! (estadoBotonOff | estadoBotonVelUp | estadoBotonVelDown | estadoBotonStop ))
    {
      //Esperamos a que se deje de cumplir esto para retonar el valor de la tecla
      estadoBotonOn = digitalRead(botonOn);
    }
    return 49;   //1  valor de on
  }

  if ( estadoBotonStop && ! (estadoBotonOff | estadoBotonOn | estadoBotonVelDown | estadoBotonVelUp )) {
    //tecla = 50/*teclaVel2*/;
    while ( estadoBotonStop && ! (estadoBotonOff | estadoBotonOn | estadoBotonVelDown | estadoBotonVelUp ))
    {
      //Esperamos a que se deje de cumplir esto para retonar el valor de la tecla
      estadoBotonStop = digitalRead(botonStop);
    }
    return 50;    //2  valor de stop
  }

  if (estadoBotonVelUp  && ! (estadoBotonOff | estadoBotonOn | estadoBotonVelDown | estadoBotonStop )) {
    while (estadoBotonVelUp  && ! (estadoBotonOff | estadoBotonOn | estadoBotonVelDown | estadoBotonStop ))
    {
      //Esperamos a que se deje de cumplir esto para retonar el valor de la tecla
      estadoBotonVelUp = digitalRead(botonVelUp);
      delay(100);
      return 51;
    }
    return 51;   //3  valor de up
  }

  if (estadoBotonVelDown && ! (estadoBotonOff | estadoBotonOn | estadoBotonVelUp | estadoBotonStop )) {
    while (estadoBotonVelDown && ! (estadoBotonOff | estadoBotonOn | estadoBotonVelUp | estadoBotonStop ))
    {
      //Esperamos a que se deje de cumplir esto para retonar el valor de la tecla
      estadoBotonVelDown  = digitalRead(botonVelDown);
      delay(100);
      return 52;
    }
    return 52;   //4   valor de down
  }

  return 0;
}

//    FUNCIONES ROBADAS


void togglePin(int pin) {
  digitalWrite(pin, LOW);
  digitalWrite(pin, HIGH);
  delay(retardoToggle);
  digitalWrite(pin, LOW);
  tone(buzzerPin, 1500, retardoToggle);
  delay(retardoToggle);
}


void printDisplay(int N) {
  if (N < 0) {
    digitalWrite(LatchPin, 0);
    shiftOut(SerialPin, ClockPin, LSBFIRST,  digito[10]);
    shiftOut(SerialPin, ClockPin, LSBFIRST,  digito[10]);
    shiftOut(SerialPin, ClockPin, LSBFIRST,  digito[10]);
    digitalWrite(LatchPin, 1);
    delay(10);
    return;
  }
  short int unidad = N % 10;
  short int decena = (N / 10) % 10;
  short int centena = (N / 100) % 10;
  if (centena == 0) {
    centena = 10;
  }

  digitalWrite(LatchPin, 0);
  shiftOut(SerialPin, ClockPin, LSBFIRST,  digito[unidad]);
  shiftOut(SerialPin, ClockPin, LSBFIRST,  digitoDot[decena]);
  shiftOut(SerialPin, ClockPin, LSBFIRST,  digito[centena]);
  digitalWrite(LatchPin, 1);
  delay(10);

}


void setInput() {
  pinMode(botonOn,      0);
  pinMode(botonOff,     0);
  pinMode(botonVelUp,   0);
  pinMode(botonVelDown, 0);
  pinMode(botonStop,    0);
}

void setOutput() {
  pinMode(SerialPin, 1);
  pinMode(ClockPin,  1);
  pinMode(LatchPin,  1);
  pinMode(VelPinDown, 1);
  pinMode(OffPin,    1);
  pinMode(VelPinUp,  1);
  pinMode(OnPin,     1);
  pinMode(StopPin,   1);
  pinMode(buzzerPin, 1);
  pinMode(TxRxPin,   1);

}

