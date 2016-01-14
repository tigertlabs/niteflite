/**
Simple "ICAO style" Navigation Lights for RC aircraft using Arduino Nano.

Copyright (C) 2016 Tuomas Kuosmanen

Adapted from "Erics DIY Aircraft Navigation Lights Project" http://mkme.org

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

*/

// Landing light settings
#define IRQ_NUMBER 0 // Interrupt number to use (0 = pin 2 on most boards)
#define PIN_SERVO 2 // Servo input pin number - this needs to match whatever interrupt is used
#define SERVO_LOW 1000 // Switch low threshold
#define SERVO_HIGH 1800 // Switch high threshold
#define SERVO_REVERSED true   // Whether or not the servo channel is reversed

// Strobe settings
#define STB1_PIN_LIGHT 3 // Landing light output pin number
#define STB2_PIN_LIGHT 4 // Pin number for strobe light output
#define STB_BLINK_INTERVAL 2000000 // Blink interval for strobe light in microseconds

// Anti-collision beacon settings
#define ACB1_PIN_LIGHT 5 // Pin number for anti-collision beacon 1
#define ACB2_PIN_LIGHT 6 // Pin number for anti-collision beacon 2
#define ACB_FADE_MIN 0 // Minimum fade level for beacon (0-255)
#define ACB_FADE_MAX 255 // Maximum fade level for beacon (0-255)
#define ACB_FADE_INTERVAL 4000 // Fade step interval, in microseconds (lower numbers = faster fade)

// Nav lights (tigert)
#define NAVL_PIN_LIGHT 7  // Landing light output pin number
#define NAVR_PIN_LIGHT 8  // Landing light output pin number
#define NAVB_PIN_LIGHT 9  // Landing light output pin number

// Var declarations
volatile unsigned long servoPulseStartTime;
volatile int servoPulseWidth = 0;

// strobe and nav via servo, beacon is on whenever we have power!
boolean switchStrobeLight = false;
boolean switchNavLight = false;

unsigned long lastFadeTime = 0;
unsigned long lastStrobeTime = 0;
int currentFade = ACB_FADE_MIN;
int fadeDirection = 1;

// Called on power on or reset
void setup()
{
  // Set up interrupt handler. 
  // FIXME: currently off and hardcoded
  // to 2000, you can change the value above to test the logic
  // without a RC receiver connected.
  // attachInterrupt(IRQ_NUMBER, measureServoSignal, CHANGE);
  servoPulseWidth = 2000;

  // Declare output pins
  pinMode(STB1_PIN_LIGHT, OUTPUT);
  pinMode(STB2_PIN_LIGHT, OUTPUT);
  pinMode(ACB1_PIN_LIGHT, OUTPUT);
  pinMode(ACB2_PIN_LIGHT, OUTPUT);
  pinMode(NAVL_PIN_LIGHT, OUTPUT);
  pinMode(NAVR_PIN_LIGHT, OUTPUT);
  pinMode(NAVB_PIN_LIGHT, OUTPUT);
  
}

// Called continuouly
void loop()
{
  unsigned long currentTime = micros();

  checkServo();

  setNavLight(switchNavLight);
   
  // Check if it's time to fade the anti-collision lights. Strobes are always on.
  if ((currentTime - lastFadeTime) > ACB_FADE_INTERVAL) {
    doFade();
    lastFadeTime = currentTime;
  }

  // Check if it's time to blink the strobes
  if ((currentTime - lastStrobeTime) > STB_BLINK_INTERVAL && switchStrobeLight) {
    doStrobe();
    lastStrobeTime = currentTime; 
  }
}

// Check servo signal, and decide to turn on/off the landing light
void checkServo()
{

  // Check servo position
  if (servoPulseWidth <= SERVO_LOW) { // everything off
    switchNavLight = false;
    switchStrobeLight = false;
  } else if (servoPulseWidth <= SERVO_HIGH) { // nav only
    switchNavLight = true;
    switchStrobeLight = false;
  } else {
    switchNavLight = true;
    switchStrobeLight = true;
  }
}

// Turn on or off nav lights
void setNavLight(boolean state)
{
  float i;
  if (state) {
    digitalWrite(NAVL_PIN_LIGHT, HIGH);
    digitalWrite(NAVR_PIN_LIGHT, HIGH);
    digitalWrite(NAVB_PIN_LIGHT, HIGH);
  } else {
    digitalWrite(NAVL_PIN_LIGHT, LOW);
    digitalWrite(NAVR_PIN_LIGHT, LOW);
    digitalWrite(NAVB_PIN_LIGHT, LOW);
  }
}

// Fade anti-collision LEDs
void doFade()
{
  currentFade += fadeDirection;
  if (currentFade == ACB_FADE_MAX || currentFade == ACB_FADE_MIN) {
    // If we hit the fade limit, flash the high beacon, and flip the fade direction
    if (fadeDirection == 1) {
      analogWrite(ACB1_PIN_LIGHT, 255);

    } else {
      analogWrite(ACB2_PIN_LIGHT, 255);
    }
    delay(50); 
    fadeDirection *= -1; 
  }

  analogWrite(ACB1_PIN_LIGHT, currentFade);
  analogWrite(ACB2_PIN_LIGHT, ACB_FADE_MAX - currentFade + ACB_FADE_MIN);
}

// Strobe double-blink
void doStrobe()
{
  digitalWrite(STB1_PIN_LIGHT, HIGH);
  delay(50);
  digitalWrite(STB1_PIN_LIGHT, LOW);
  delay(50);
  digitalWrite(STB1_PIN_LIGHT, HIGH);
  delay(50);
  digitalWrite(STB1_PIN_LIGHT, LOW);
  delay(100);
  digitalWrite(STB2_PIN_LIGHT, HIGH);
  delay(50);
  digitalWrite(STB2_PIN_LIGHT, LOW);
  delay(50);
  digitalWrite(STB2_PIN_LIGHT, HIGH);
  delay(50);
  digitalWrite(STB2_PIN_LIGHT, LOW);
}

// Measure servo PWM signal
void measureServoSignal()
{
  int pinState = digitalRead(PIN_SERVO);
  if(pinState == HIGH) { 
    // Beginning of PWM pulse, mark time
    servoPulseStartTime = micros();
  } else {
    // End of PWM pulse, calculate pulse duration in mcs
    servoPulseWidth = (int)(micros() - servoPulseStartTime);

    // If servo channel is reversed, use the inverse
    if (SERVO_REVERSED) {
      servoPulseWidth = (1000 - (servoPulseWidth - 1000)) + 1000;
    }
  }
}
