// -----------------------------------------
// Copyright 2018 Ian Maines
// All Rights Reserved
// -----------------------------------------
// This app generates bogus temperature and humidity data for testing various integrations
// (ie AWS IoT, IFTTT)

#include <Wire.h>
#include "SparkFunTMP102.h"


int PIN_boardLed = D7; // On-board LED
double InternalTemperature = 68;

// We start with the setup function.

// Global TMP201 variable
TMP102 _temp_sensor (0x48); // Default address initialization

void setup()
  {
  // Configure the LED pin
  pinMode(PIN_boardLed,OUTPUT); // Our on-board LED is output as well

  // Turn the LED off
  digitalWrite(PIN_boardLed,HIGH);

  _temp_sensor.begin ();

  // set the Conversion Rate (how quickly the sensor gets a new reading)
  //0-3: 0:0.25Hz, 1:1Hz, 2:4Hz, 3:8Hz
  _temp_sensor.setConversionRate(2);

  // Publish that we've rebooted
  Particle.publish("DeviceStatus","boot",60,PUBLIC);
  Particle.variable ("InsideTemp", InternalTemperature);
  }


// Now for the loop.

void loop()
  {
  digitalWrite(PIN_boardLed, HIGH);
  delay(500);
  digitalWrite(PIN_boardLed, LOW);
  delay(500);
  digitalWrite(PIN_boardLed, HIGH);
  delay(500);
  digitalWrite(PIN_boardLed, LOW);
  delay(500);
  digitalWrite(PIN_boardLed, HIGH);
  delay(500);
  digitalWrite(PIN_boardLed, LOW);
  delay(500);
  double shift = (double)random(0,999)/1000.0;
  bool posneg = random(1,100) >= 50;
  // Turn sensor on to start temperature measurement.
  // Current consumtion typically ~10uA.
  _temp_sensor.wakeup();
  InternalTemperature = _temp_sensor.readTempF();
  _temp_sensor.sleep();

  Particle.publish("InternalTemperature", String(InternalTemperature),60,PUBLIC);
  }
