/*
# JoyStickMatico V2.0
# JoyStick controlado por Arduino, principalmente para su uso en Simuladores (Farming Simulator)
Desarrollado con Visual Code + PlatformIO en Framework Arduino
Desarrollado por Diego Maroto, Febrero 2022 - dmarofer@diegomaroto.net
https://bilbaomakers.org/
Licencia: GNU General Public License v3.0 - https://www.gnu.org/licenses/gpl-3.0.html
*/

/*

Desarrollado con la idea de multiplicar el numero de ejes y botones cuando es necesario tenerlos pero no
utilizarlos a la vez.

El control fisico dispone de un JoyStick de 3 ejes y un boton
Tambien dispone de una botonera matriz de 4x4 (no necesitan diodo)
Al Sistema operativo le presenta 6 ejes y 21 botones

Mapa de Controles Logicos. referencia de los botones en el sistema operativo. En la libreria el primer indice es 0

MODO 1: LED APAGADO
MODO 2: LED ENCENDIDO
Haciendo HOLD (pulsado mas del tiempo configurado) el boton del JoyStick se alterna el modo de trabajo.
Fuera del modo los ejes logicos presentan estado centrado (0,0,0)

X,Y,Z - Ejes del JoyStick en modo 1
Rx,Ry,Rz - Ejes del JoyStick en modo 2
B1 - Click del boton fisico del JoyStick en cualquiera de los 2 modos
B2 - Doble click del boton del JoyStick en modo 1
B3 - Triple click del boton del JoyStick en modo 1
B4 - Doble click del boton del JoyStick en modo 2
B5 - Triple click del boton del JoyStick en modo 2
B[6-21] - Matriz 4x4 (123A456B789C*0#D)

B1, B2 y B4 emulan automaticamente una pulsacion (se desactivan al tiempo estbalecido)
B3 y B5 Cambian de estado cada vez que se activan (toggle)
B[6-21] Se pueden pulsar hasta 10 simultaneamente y reflejan el estado fisico del boton siempre (pulsado o no)

*/


#pragma region LIBS

#include <Arduino.h>
#include "Joystick.h"
#include <OneButton.h> // Interesante: https://github.com/mathertel/OneButton
#include <jled.h> // Para los led, a ver si rula esta vez ....
#include <Keypad.h> //Para el teclado 4x4

#pragma endregion

#pragma region CONFIG

// Hardware
#define PIN_EJE_X A1
#define PIN_EJE_Y A0
#define PIN_EJE_Z A2
#define PIN_BOTON_1 15
#define PIN_LED_1 10

//Para el teclado 4x4
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the kpd
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the kpd

// Array con variables internas para el estado de los botones del teclado
bool EstadoBotonesTeclado[16] = {false};


#define CLEARTIME 100 // ms antes de apagar los botones logicos despues de un cambio

#pragma endregion

#pragma region DEFINICIONES

// Objeto del Joystick Logico
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 21, 0,
  true, true, true, true, true, true,
  false, false, false, false, false);

// Objeto para el boton fisico del JoyStick
OneButton Boton1 = OneButton(
  PIN_BOTON_1,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

//Objeto para el teclado 4x4
//MD_UISwitch_Matrix Teclado(ROWS, COLS, rowPins, colPins, kt);
Keypad Teclado = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Objeto para el led
JLed Led1 = JLed(PIN_LED_1);

// Variables internas
unsigned long ChangeTime = 0; // Variable para los millis cuando hay un cambio de boton
bool Modo2 = false; // Variable para saber si el Joystick esta en Modo1 o Modo2
bool EstadoBoton3 = false, EstadoBoton5 = false; // Estado interno para los botones toogle

// ############################################
// FUNCIONES DE MANEJO DEL BOTON

#pragma endregion

#pragma region CALLOUTS

// Manejador para el evento 1 click
static void handleClick() {
   
  Joystick.pressButton(0);
  ChangeTime = millis();

}

// Manejador para el evento 2 clicks
static void handleDoubleClick() {
  
  if(!Modo2){

    Joystick.pressButton(1);

  }
  
  else{

    Joystick.pressButton(3);

  }
  
  ChangeTime = millis();

}

// Manejador para el evento HOLD
static void handleLongPressStart() {
  
  Modo2 = !Modo2;

}

// Manejador para el evento MultiClick
static void handleMultiClick() {

  switch (Boton1.getNumberClicks())
  {
  case 3:
    
    if(!Modo2){

      EstadoBoton3 = !EstadoBoton3;

    }
  
    else{

      EstadoBoton5 = !EstadoBoton5;

    }

    break;
  
  default:
    break;
  }

}

#pragma endregion

#pragma region ARDUINOCODE

void setup() {

  // Inicializacion de variables
  ChangeTime = 0;
  Modo2 = false;
  EstadoBoton3 = false;
  EstadoBoton5 = false;

  // Configuracion del rango de los ejes logicos
  Joystick.setXAxisRange(-127,127);
  Joystick.setYAxisRange(-127,127);
  Joystick.setZAxisRange(-127,127);
  Joystick.setRxAxisRange(-127,127);
  Joystick.setRyAxisRange(-127,127);
  Joystick.setRzAxisRange(-127,127);
  
  // Inicio del objeto Joystick
  Joystick.begin();

  // Asignacion de callouts a los eventos del Boton del JoyStick
  Boton1.attachClick(handleClick);
  Boton1.attachDoubleClick(handleDoubleClick);
  Boton1.attachLongPressStart(handleLongPressStart);
  Boton1.attachMultiClick(handleMultiClick);
  
  // Configuracion de tiempos
  Boton1.setDebounceTicks(20); // ms de debounce
  Boton1.setClickTicks(150); // ms para deteccion de multiclick
  Boton1.setPressTicks(300); // ms para HOLD

  // Configuracion del Hardware
  pinMode(PIN_BOTON_1, INPUT_PULLUP);
  pinMode(PIN_LED_1, OUTPUT);

  //Teclado
  Teclado.setDebounceTime(15);
  Teclado.setHoldTime(1000);
    
  // Puerto serie. Fin del setup
  Serial.begin(9600);
  Serial.println("##SETUP COMPLETADO");
  Led1.Blink(200,200).Repeat(5);

}

void loop() {

  // Funciones de vida de los objetos
  Led1.Update();
  Boton1.tick();
  
  // Lectura del Teclado. Botones 6-21 (Indice 5-20).
  if (Teclado.getKeys()){
  
    
    
    for (int i=0; i<LIST_MAX; i++){         // Bucle para escanear todo el buffer (10)
    
      if ( Teclado.key[i].stateChanged ){   // Si la tecla ha cambiado de estado
      
        switch (Teclado.key[i].kstate) {    // Mirar su estado actual
    
            case PRESSED:
            Joystick.pressButton(Teclado.key[i].kcode + 5);      // Actualizar en el objeto JoyStick. Offset de indice +4
            break;
            
            case RELEASED:
            Joystick.releaseButton(Teclado.key[i].kcode + 5);    // Actualizar en el objeto JoyStick. Offset de indice +4
            break;
            
            default:
            break;
        }

      }

    }

  }

  // Actualizacion de estados de los ejes logicos segun estado de los fisicos y segun el modo
  if (!Modo2){

    // Modo Ejes Normal
    Led1.Off();
    Joystick.setXAxis(map(analogRead(PIN_EJE_X),280,743,-127,127));
    Joystick.setYAxis(map(analogRead(PIN_EJE_Y),280,743,-127,127));
    Joystick.setZAxis(map(analogRead(PIN_EJE_Z),200,823,127,-127));
    Joystick.setRxAxis(0);
    Joystick.setRyAxis(0);
    Joystick.setRzAxis(0);

  }

  else{

    // Modo alternativo Ejes
    Led1.Blink(200,200).Forever();
    Joystick.setXAxis(0);
    Joystick.setYAxis(0);
    Joystick.setZAxis(0);
    Joystick.setRxAxis(map(analogRead(PIN_EJE_X),280,743,-127,127));
    Joystick.setRyAxis(map(analogRead(PIN_EJE_Y),280,743,-127,127));
    Joystick.setRzAxis(map(analogRead(PIN_EJE_Z),200,823,127,-127));

  }

  // Actualizr estado de los botones Toggle
  Joystick.setButton(2,EstadoBoton3);
  Joystick.setButton(4,EstadoBoton5);

  // Para apagar los botones logicos una vez transcurrido X tiempo desde el encendido
  if (ChangeTime != 0 && (millis() - ChangeTime) > CLEARTIME ){
    
    Joystick.releaseButton(0);
    Joystick.releaseButton(1);
    Joystick.releaseButton(3);
    ChangeTime=0;
        
  }

  // Enviar los datos al PC
  Joystick.sendState();

}

#pragma endregion