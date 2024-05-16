#include <Arduino.h>
#include <pitches.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// run after start
void setup() 
{
  // serial monitor setup
  Serial.begin(9600);
  Serial.println("Start");
}

// run forever until break
void loop() 
{
  Serial.println("Loop started");
  delay(1000);
}
