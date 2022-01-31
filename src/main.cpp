#include <Arduino.h>
#include "Joystick.h"
#include <OneButton.h> // Interesante: https://github.com/mathertel/OneButton

#define PIN_EJE_X A1
#define PIN_EJE_Y A0
#define PIN_EJE_Z A2
#define PIN_BOTON_1 15
#define PIN_LED_1 9

#define CLEARTIME 100 // ms antes de apagar los botones logicos despues de un cambio

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 3, 0,
  true, true, false, true, false, false,
  false, false, false, false, false);

OneButton Boton1 = OneButton(
  PIN_BOTON_1,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

unsigned long ChangeTime = 0; // Variable para los millis cuando hay un cambio de boton

// ############################################
// FUNCIONES DE MANEJO DEL BOTON

static void handleClick() {
  Joystick.pressButton(0);
  ChangeTime = millis();
}

static void handleDoubleClick() {
  Joystick.pressButton(1);
  ChangeTime = millis();
}

static void handleLongPressStart() {
  Joystick.pressButton(2);
  ChangeTime = millis();
}

/*
static void handleMultiClick() {
  Serial.println("Clicked!");
}

static void handleDuringLongPress() {
  Serial.println("Clicked!");
}

static void handleLongPressStop() {
  Serial.println("Clicked!");
}
*/

// ############################################

void setup() {

  Joystick.setXAxisRange(-127,127);
  Joystick.setYAxisRange(-127,127);
  Joystick.setRxAxisRange(-127,127);
  
  Joystick.begin();

  pinMode(PIN_BOTON_1, INPUT_PULLUP);
  pinMode(PIN_LED_1, OUTPUT);

  Boton1.attachClick(handleClick);
  Boton1.attachDoubleClick(handleDoubleClick);
  Boton1.attachLongPressStart(handleLongPressStart);
  
  Boton1.setDebounceTicks(20); // ms de debounce
  Boton1.setClickTicks(200); // ms para deteccion de multiclick
  Boton1.setPressTicks(1000); // ms para HOLD

  Serial.begin(9600);
  Serial.println("##SETUP COMPLETADO");

}

void loop() {

  Boton1.tick();

  // Ejes
  Joystick.setXAxis(map(analogRead(PIN_EJE_X),280,743,-127,127));
  Joystick.setYAxis(map(analogRead(PIN_EJE_Y),280,743,-127,127));
  Joystick.setRxAxis(map(analogRead(PIN_EJE_Z),200,823,127,-127));
  Joystick.sendState();
  
  // Para apagar los botones logicos una vez transcurrido X tiempo desde el encendido
  if ((millis() - ChangeTime) > CLEARTIME && ChangeTime != 0){

    Joystick.releaseButton(0);
    Joystick.releaseButton(1);
    Joystick.releaseButton(2);
    Joystick.sendState();
    ChangeTime=0;
    
  }

}