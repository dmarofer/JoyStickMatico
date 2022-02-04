/*
# JoyStickMatico V1.0
# JoyStick controlado por Arduino, principalmente para su uso en Simuladores (Farming Simulator)
Desarrollado con Visual Code + PlatformIO en Framework Arduino
Desarrollado por Diego Maroto, Enero 2022 - dmarofer@diegomaroto.net
https://bilbaomakers.org/
Licencia: GNU General Public License v3.0 - https://www.gnu.org/licenses/gpl-3.0.html
*/

/*

Desarrollado con la idea de multiplicar el numero de ejes y botones cuando es necesario tenerlos pero no
utilizarlos a la vez.

El control fisico dispone de un JoyStick de 3 ejes y un boton
Al Sistema operativo le presenta 6 ejes y 6 botones que funcionan Asi

MODO 1 (Modo "Normal" - led apagado)
  - Con el Joystick movemos los ejes logicos X,Y,Z
  - Con el boton click sencillo se hace click en el boton logico 1
  - Con el boton doble click se hace click en el boton logico 2
  - Con el boton triple click se alterna el estado del boton logico 3

MODO 2 (Modo "Shift" - led encendido)
  - Con el Joystick movemos los ejes logicos RX,RY,RZ
  - Con el boton click sencillo se hace click en el boton logico 4
  - Con el boton doble click se hace click en el boton logico 5
  - Con el boton triple click se alterna el estado del boton logico 6

Haciendo HOLD (pulsado mas del tiempo configurado) el boton se alterna el modo de trabajo.

Fuera del modo los ejes logicos presentan estado centrado (0,0,0)

*/


#pragma region LIBS

#include <Arduino.h>
#include "Joystick.h"
#include <OneButton.h> // Interesante: https://github.com/mathertel/OneButton
#include <jled.h> // Para los led, a ver si rula esta vez ....
#include <MD_UISwitch.h> //Para el teclado 4x4

#pragma endregion

#pragma region CONFIG

// Hardware
#define PIN_EJE_X A1
#define PIN_EJE_Y A0
#define PIN_EJE_Z A2
#define PIN_BOTON_1 15
#define PIN_LED_1 10

//Para el teclado 4x4
uint8_t rowPins[] = { 5, 4, 3, 2 };     // connected to keypad row pinouts
uint8_t colPins[] = { 9, 8, 7, 6 };   // connected to the keypad column pinouts
const uint8_t ROWS = sizeof(rowPins);
const uint8_t COLS = sizeof(colPins);
char kt[(ROWS*COLS) + 1] = "123A456B789C*0#D";  //define the symbols for the keypad


#define CLEARTIME 100 // ms antes de apagar los botones logicos despues de un cambio

#pragma endregion

#pragma region DEFINICIONES

// Objeto del Joystick Logico
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 38, 0,
  true, true, true, true, true, true,
  false, false, false, false, false);

// Objeto para el boton fisico
OneButton Boton1 = OneButton(
  PIN_BOTON_1,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

//Objeto para el teclado 4x4
MD_UISwitch_Matrix Teclado(ROWS, COLS, rowPins, colPins, kt);

// Objeto para el led
JLed Led1 = JLed(PIN_LED_1);

// Variables internas
unsigned long ChangeTime = 0; // Variable para los millis cuando hay un cambio de boton
bool ModoShift = false; // Variable para saber si el Joystick esta en Modo Normal o modo Shift
bool EstadoBoton36 = false, EstadoBoton37 = false; // Estado interno para los botones toogle

// ############################################
// FUNCIONES DE MANEJO DEL BOTON

#pragma endregion

#pragma region CALLOUTS

// Manejador para el evento 1 click
static void handleClick() {
  
  if(!ModoShift){

    Joystick.pressButton(0);

  }
  
  else{

    Joystick.pressButton(2);

  }
  
  ChangeTime = millis();

}

// Manejador para el evento 2 clicks
static void handleDoubleClick() {
  
  if(!ModoShift){

    Joystick.pressButton(1);

  }
  
  else{

    Joystick.pressButton(3);

  }
  
  ChangeTime = millis();

}

// Manejador para el evento HOLD
static void handleLongPressStart() {
  
  ModoShift = !ModoShift;

}

// Manejador para el evento MultiClick
static void handleMultiClick() {

  switch (Boton1.getNumberClicks())
  {
  case 3:
    
    if(!ModoShift){

      EstadoBoton36 = !EstadoBoton36;

    }
  
    else{

      EstadoBoton37 = !EstadoBoton37;

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
  ModoShift = false;
  EstadoBoton36 = false;
  EstadoBoton37 = false;

  // Configuracion del rango de los ejes logicos
  Joystick.setXAxisRange(-127,127);
  Joystick.setYAxisRange(-127,127);
  Joystick.setZAxisRange(-127,127);
  Joystick.setRxAxisRange(-127,127);
  Joystick.setRyAxisRange(-127,127);
  Joystick.setRzAxisRange(-127,127);
  
  // Inicio del objeto Joystick
  Joystick.begin();

  // Asignacion de callouts a los eventos  
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
  Teclado.begin();
  Teclado.enableRepeat(false);
  Teclado.enableRepeatResult(false);
  Teclado.enableDoublePress(false);
  Teclado.enableLongPress(true);
  Teclado.setDoublePressTime(50);
  Teclado.setLongPressTime(100);

  // Puerto serie. Fin del setup
  Serial.begin(9600);
  Serial.println("##SETUP COMPLETADO");
  Led1.Blink(200,200).Repeat(5);

}

void loop() {

  // Funciones de vida de los objetos
  Led1.Update();
  Boton1.tick();

  //Lectura del teclado
  MD_UISwitch::keyResult_t ModoTeclaPulsada = Teclado.read();
  switch(ModoTeclaPulsada){
    
    case MD_UISwitch::KEY_PRESS:     
      
      switch ((char)Teclado.getKey()){

        case '1':
        Joystick.pressButton(4);
        break;

        case '2':
        Joystick.pressButton(5);
        break;

        case '3':
        Joystick.pressButton(6);
        break;

        case 'A':
        Joystick.pressButton(7);
        break;

        case '4':
        Joystick.pressButton(8);
        break;

        case '5':
        Joystick.pressButton(9);
        break;

        case '6':
        Joystick.pressButton(10);
        break;

        case 'B':
        Joystick.pressButton(11);
        break;

        case '7':
        Joystick.pressButton(12);
        break;

        case '8':
        Joystick.pressButton(13);
        break;

        case '9':
        Joystick.pressButton(14);
        break;

        case 'C':
        Joystick.pressButton(15);
        break;

        case '*':
        Joystick.pressButton(16);
        break;

        case '0':
        Joystick.pressButton(17);
        break;

        case '#':
        Joystick.pressButton(18);
        break;

        case 'D':
        Joystick.pressButton(19);
        break;
      
      default:
        break;
     
     }
      
    ChangeTime = millis();
    break;
    
    case MD_UISwitch::KEY_LONGPRESS: 

      switch ((char)Teclado.getKey()){

        case '1':
        Joystick.pressButton(20);
        break;

        case '2':
        Joystick.pressButton(21);
        break;

        case '3':
        Joystick.pressButton(22);
        break;

        case 'A':
        Joystick.pressButton(23);
        break;

        case '4':
        Joystick.pressButton(24);
        break;

        case '5':
        Joystick.pressButton(25);
        break;

        case '6':
        Joystick.pressButton(26);
        break;

        case 'B':
        Joystick.pressButton(27);
        break;

        case '7':
        Joystick.pressButton(28);
        break;

        case '8':
        Joystick.pressButton(29);
        break;

        case '9':
        Joystick.pressButton(30);
        break;

        case 'C':
        Joystick.pressButton(31);
        break;

        case '*':
        Joystick.pressButton(32);
        break;

        case '0':
        Joystick.pressButton(33);
        break;

        case '#':
        Joystick.pressButton(34);
        break;

        case 'D':
        Joystick.pressButton(35);
        break;
      
        default:
        break;
      }  

    ChangeTime = millis();
    break;
    
    default:                         
    break;

  }
  
  // Actualizacion de estados de los ejes logicos segun estado de los fisicos y segun el modo
  if (!ModoShift){

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

  Joystick.setButton(36,EstadoBoton36);
  Joystick.setButton(37,EstadoBoton37);
  

  // Para apagar los botones logicos una vez transcurrido X tiempo desde el encendido
  if (ChangeTime != 0 && (millis() - ChangeTime) > CLEARTIME ){

    for (int i = 0; i < 36; i++)
    {
      Joystick.releaseButton(i);
    }
    
    /*
    Joystick.releaseButton(0);
    Joystick.releaseButton(1);
    Joystick.releaseButton(2);
    Joystick.releaseButton(3);
    */

    Joystick.sendState();
    ChangeTime=0;
        
  }

  Joystick.sendState();

}

#pragma endregion