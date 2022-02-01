#pragma region LIBS

#include <Arduino.h>
#include "Joystick.h"
#include <OneButton.h> // Interesante: https://github.com/mathertel/OneButton
#include <jled.h> // Para los led, a ver si rula esta vez ....

#pragma endregion

#pragma region CONFIG

#define PIN_EJE_X A1
#define PIN_EJE_Y A0
#define PIN_EJE_Z A2
#define PIN_BOTON_1 15
#define PIN_LED_1 9

#define CLEARTIME 100 // ms antes de apagar los botones logicos despues de un cambio

#pragma endregion

#pragma region CALLOUTS

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 6, 0,
  true, true, true, true, true, true,
  false, false, false, false, false);

OneButton Boton1 = OneButton(
  PIN_BOTON_1,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

JLed Led1 = JLed(PIN_LED_1);

unsigned long ChangeTime = 0; // Variable para los millis cuando hay un cambio de boton

bool EjesRotacion = false; // Variable para saber si el modo ejes esta en normal o alternativos
bool EstadoBoton4 = false, EstadoBoton5 = false; // Estado interno para los botones toogle

// ############################################
// FUNCIONES DE MANEJO DEL BOTON

static void handleClick() {
  
  if(!EjesRotacion){

    Joystick.pressButton(0);

  }
  
  else{

    Joystick.pressButton(2);

  }
  
  ChangeTime = millis();

}

static void handleDoubleClick() {
  
  if(!EjesRotacion){

    Joystick.pressButton(1);

  }
  
  else{

    Joystick.pressButton(3);

  }
  
  ChangeTime = millis();

}

static void handleLongPressStart() {
  
  EjesRotacion = !EjesRotacion;

}


static void handleMultiClick() {

  switch (Boton1.getNumberClicks())
  {
  case 3:
    
    if(!EjesRotacion){

      EstadoBoton4 = !EstadoBoton4;

    }
  
    else{

      EstadoBoton5 = !EstadoBoton5;

    }

    break;
  
  default:
    break;
  }

}

/*
static void handleDuringLongPress() {
  Serial.println("Clicked!");
}

static void handleLongPressStop() {
  Serial.println("Clicked!");
}
*/

// ############################################

#pragma endregion

#pragma region ARDUINOCODE

void setup() {

  ChangeTime = 0;
  EjesRotacion = false;
  EstadoBoton4 = false;
  EstadoBoton5 = false;


  Joystick.setXAxisRange(-127,127);
  Joystick.setYAxisRange(-127,127);
  Joystick.setZAxisRange(-127,127);
  Joystick.setRxAxisRange(-127,127);
  Joystick.setRyAxisRange(-127,127);
  Joystick.setRzAxisRange(-127,127);
  
  Joystick.begin();

  pinMode(PIN_BOTON_1, INPUT_PULLUP);
  pinMode(PIN_LED_1, OUTPUT);

  Boton1.attachClick(handleClick);
  Boton1.attachDoubleClick(handleDoubleClick);
  Boton1.attachLongPressStart(handleLongPressStart);
  Boton1.attachMultiClick(handleMultiClick);
    
  Boton1.setDebounceTicks(20); // ms de debounce
  Boton1.setClickTicks(150); // ms para deteccion de multiclick
  Boton1.setPressTicks(300); // ms para HOLD

  Serial.begin(9600);
  Serial.println("##SETUP COMPLETADO");

  Led1.Blink(200,200).Repeat(5);

}

void loop() {

  Led1.Update();

  Boton1.tick();

  if (!EjesRotacion){

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

  Joystick.setButton(4,EstadoBoton4);
  Joystick.setButton(5,EstadoBoton5);

  Joystick.sendState();

  // Para apagar los botones logicos una vez transcurrido X tiempo desde el encendido
  if (ChangeTime != 0 && (millis() - ChangeTime) > CLEARTIME ){

    Joystick.releaseButton(0);
    Joystick.releaseButton(1);
    Joystick.releaseButton(2);
    Joystick.releaseButton(3);
    Joystick.sendState();
    ChangeTime=0;
    
    
  }

}

#pragma endregion