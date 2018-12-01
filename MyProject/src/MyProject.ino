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
  _temp_sensor.setConversionRate(1);
  _temp_sensor.wakeup();

  // Publish that we've rebooted
  Particle.publish("DeviceStatus","boot",60,PUBLIC);
  Particle.variable ("InsideTemp", InternalTemperature);
  }


// Now for the loop.

void loop()
  {
  if (Particle.connected())
    {
    // Turn the on-board LED on while we;re processing.
    digitalWrite(PIN_boardLed, HIGH);
    _temp_sensor.wakeup();	// Wakeup and start running in normal power mode
    // Turn sensor on to start temperature measurement.
    // Current consumtion typically ~10uA.
    InternalTemperature = _temp_sensor.readTempF() - 10.19889; // Correction factor due to getting odd reading
    _temp_sensor.sleep();	// Switch sensor to low power mode

    Particle.publish("InternalTemperature", String(InternalTemperature),60,PUBLIC);

    // Make sure the on-board LED is on for at least 500ms
    delay(500);
    digitalWrite(PIN_boardLed, LOW);
    }
    // Stay awake for another 30 seconds to round out to 5 minutes
    delay((28 * 1000) + 500);
    // Flash the LED to let us know we're going to sleep
    for (int i = 0; i < 10; ++i)
      {
      digitalWrite(PIN_boardLed, HIGH);
      delay(50);
      digitalWrite(PIN_boardLed, LOW);
      delay(50);
      }
    // Put the device to sleep for 4 minutes, 30 seconds.
    Particle.sleep (SLEEP_MODE_DEEP, ((60*4)+30));
  }
