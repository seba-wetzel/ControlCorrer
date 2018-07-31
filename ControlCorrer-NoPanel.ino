#include <FreeRTOS_AVR.h>
#include "definiciones.h"


uint8_t velocidad     = 0;
bool estadoEncendido = false;
bool estadoConexion     = false;

// Flag para iniciar el cronometro
bool startCounter   = true;
bool startCountUp   = true;
bool startCountDown = false;

// Estado vertical de la cinta
estadoVertical_e estadoUpDown = down;

#define VelInicial 0;


// Cola
QueueHandle_t cola_entrada;
QueueHandle_t cola_estados;

//Semaforos
SemaphoreHandle_t semaforo_boton;

//Prototipo de funcion task
void TareaTimer (void *pvParameters);
void TareaSalida(void *pvParameters);
void TareaSerial(void *pvParameters);

// uint8_t getTecla(void);
 uint8_t funcionSerial(void);
//void printDisplay(int);



void setup() {

  //setInput();
  setOutput();
  //printDisplay(-1);
  Serial.begin(57600);
  DEBUG_PRINTLN ("arrancamos");
  //Colas
  cola_entrada = xQueueCreate(tamCola, tamMsg);   //Manejador de la cola delas entradas

  //Semaforos
  //vSemaphoreCreateBinary(semaforo_boton );

  //Tareas

  xTaskCreate(TareaTimer,  "Tarea timer", tamPila, NULL, prioridadTimer,  NULL);
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

void TareaTimer (void *pvParameters) {
 volatile uint16_t time;
 volatile uint8_t mensaje[2];
 uint16_t tempTime;

  while (1) {

    if (velocidad == 0){
      startCountUp = true; 
      startCounter = true;
      startCountDown = false;
    }

    tempTime = millis();
    if (velocidad > 0 && startCounter){
    time = millis();
    startCounter = false;
    }
    
    if (( ( (tempTime - time ) / 1000) >= timeToTurnUp) & !startCounter & startCountUp){
      mensaje[0] = 85;
      xQueueSend(cola_entrada, &mensaje, 100);
      startCountUp = false; 
      startCountDown = true;     
    }

    if (( ( (tempTime - time ) / 1000) >= timeToTurnDown) & !startCounter & startCountDown){
      mensaje[0] = 50;
      startCountUp = true; 
      startCounter = true;
      startCountDown = false;
      xQueueSend(cola_entrada, &mensaje, 100);  
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
          //printDisplay(-1);
          togglePin(StopPin);
           
          Serial.println("OK");
           
          break;
        case '0': if (estadoEncendido) {
            estadoEncendido = false;
            if (estadoConexion) {
               
              Serial.println('L');
               
            }
            velocidad = VelInicial;
            togglePin(OffPin);
            //printDisplay(-1);
          }
          break;
        case '1': if ((!estadoEncendido)) {
            estadoEncendido = true;
            //printDisplay(velocidad);
            if (estadoConexion) {
               
              Serial.println('H');
               
            }
            togglePin(OnPin);
          }
          break;
        case '2': if (estadoEncendido && velocidad > 0) {
            velocidad = VelInicial;
            //printDisplay(velocidad);
            if (estadoConexion) {
               
              Serial.println('S');
               
            }
            digitalWrite(upPin, !ReleDriverState);
            estadoUpDown = down;
            togglePin(StopPin);
          }
          break;
        case '3': if (estadoEncendido && (velocidad < limiteSuperior)) {
            ++velocidad;
            //printDisplay(velocidad);
            if (estadoConexion) {
               
              Serial.println(velocidad);
               
            }

            togglePin(VelPinUp);
          }
          break;
        case '4': if (estadoEncendido && (velocidad > 0)) {
            --velocidad;
            //printDisplay(velocidad);
            if (estadoConexion) {
               
              Serial.println(velocidad);
               
            }

            togglePin(VelPinDown);
          }
          break;

          case 'U': if ((estadoEncendido) & (estadoUpDown == down)) {
            
            if (estadoConexion) {
               
              Serial.println('U');
               
            }
            digitalWrite(upPin, ReleDriverState);
            estadoUpDown = up;
          }
          break;

          case 'D': if ((estadoEncendido) & (estadoUpDown == up)) {
            
            if (estadoConexion) {
               
              Serial.println('D');
               
            }
            digitalWrite(upPin, !ReleDriverState);
            estadoUpDown = down;
          }
          break;          

        default:
           
          Serial.println("ack");
           
          break;
      }
    }
  }
}


 uint8_t funcionSerial( void) {
  uint8_t entradaSerial = 0;
   
  if (Serial.available() > 0) {
    entradaSerial = Serial.read();
    //digitalWrite(LED_PIN, HIGH);
  }
  return entradaSerial;
}

void togglePin(int pin) {
  digitalWrite(pin, LOW);
  digitalWrite(pin, HIGH);
  delay(retardoToggle);
  digitalWrite(pin, LOW);
  //tone(buzzerPin, 1500, retardoToggle);
  delay(retardoToggle);
}

void setOutput() {
  pinMode(VelPinDown, 1);
  pinMode(OffPin,    1);
  pinMode(VelPinUp,  1);
  pinMode(OnPin,     1);
  pinMode(StopPin,   1);
  pinMode(upPin, 1);

  //Seteamos los dos reles apagados
  digitalWrite(upPin, !ReleDriverState);

}

