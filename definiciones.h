//#define DEBUG             //Definimos el flag de debug



//Macro de debug
#ifdef DEBUG
#define DEBUG_PRINTLN(x)  Serial.println (x)
#else
#define DEBUG_PRINTLN(x)
#endif

#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.print (x)
#else
#define DEBUG_PRINT(x)
#endif
//Fin de la macro

//Macro de cambio de modo Tx/Rx
//#define TxRxPin       2
//#define modoEscribir {delay(10);digitalWrite(TxRxPin,HIGH);}
//#define modoEscuchar {delay(10);digitalWrite(TxRxPin,LOW);}
//Fin de la macro

//Definiciones de las colas

#define tamCola         1
#define tamMsg          3
//Fin de definiciones de colas

//Prioridad de Tareas
#define prioridadSalida 1  //Las prioridades van de menor 
#define prioridadSerial 2  //a mayor
#define prioridadTimer  3
#define tamPila         256

//Fin de prioridades

#define retardoToggle 60
#define limiteSuperior 160

// Tiempos para subir y bajar expresados en segundos
#define timeToTurnUp 30
#define timeToTurnDown 40


//Pines de entradas
//#define botonOn      44
//#define botonOff     42
//#define botonVelUp   36
//#define botonVelDown 38
//#define botonStop    40
//Fin de entradas



//Salidas
//#define DOWN          2
#define upPin         3
#define OffPin        4
#define VelPinUp      6
#define OnPin         5
#define VelPinDown    7
#define StopPin       8

#define ReleDriverState 0


 typedef enum {
  down,
  up
 } estadoVertical_e;

//#define SerialPin     52
//#define ClockPin      50
//#define LatchPin      48

//#define buzzerPin     46

//Fin de salidas


//byte digito[11] =
//{
//  0b11100111,    // 0
//  0b00100001,    // 1
//  0b11001011,    // 2
//  0b01101011,    // 3
//  0b00101101,    // 4
//  0b01101110,    // 5
//  0b11101110,    // 6
//  0b00100011,    // 7
//  0b11101111,    // 8
//  0b00101111,    // 9
//  0b00000000     // todo apagado
//};
//
//byte digitoDot[11] =
//{
//  0b11110111,    // 0
//  0b00110001,    // 1
//  0b11011011,    // 2
//  0b01111011,    // 3
//  0b00111101,    // 4
//  0b01111110,    // 5
//  0b11111110,    // 6
//  0b00110011,    // 7
//  0b11111111,    // 8
//  0b00111111,    // 9
//  0b00000000     // todo apagado
//};
