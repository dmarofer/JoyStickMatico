#include <Arduino.h>
#include "Joystick.h"
#include <Bounce2.h>

#define PIN_EJE_X A1
#define PIN_EJE_Y A0
#define PIN_EJE_Z A2
#define PIN_BOTON_1 15
#define PIN_LED_1 9

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 2, 0,
  true, true, false, true, false, false,
  false, false, false, false, false);

Bounce2::Button bounce_button_1 = Bounce2::Button();

void setup() {

  Joystick.setXAxisRange(-127,127);
  Joystick.setYAxisRange(-127,127);
  Joystick.setRxAxisRange(-127,127);
  
  Joystick.begin();

  pinMode(PIN_BOTON_1, INPUT_PULLUP);
  pinMode(PIN_LED_1, OUTPUT);

  bounce_button_1.attach(PIN_BOTON_1, INPUT_PULLUP);
  bounce_button_1.interval(10);
  bounce_button_1.setPressedState(LOW);

  Serial.begin(9600);
  Serial.println("##SETUP COMPLETADO");

}

void loop() {

  Joystick.setXAxis(map(analogRead(PIN_EJE_X),280,743,-127,127));
  Joystick.setYAxis(map(analogRead(PIN_EJE_Y),280,743,-127,127));
  Joystick.setRxAxis(map(analogRead(PIN_EJE_Z),200,823,127,-127));
  bounce_button_1.update();
  Joystick.setButton(0,!bounce_button_1.read());
  Joystick.sendState();

  // delay(10);

}