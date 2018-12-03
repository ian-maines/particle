// -----------------------------------------
// Copyright 2018 Ian Maines
// All Rights Reserved
// -----------------------------------------
// This app generates bogus temperature and humidity data for testing various integrations
// (ie AWS IoT, IFTTT)

#include <Wire.h>
#include "SparkFunTMP102.h"

// Pin Configurations
int PIN_boardLed = D7; // On-board LED
// Use D3 as low power enable mode. Set to ground to disable.
// The goal is to not put the device to sleep when disabled so that firmware updates may be flashed.
int PIN_LowPowerEnable = D3;

// Exposed variable Configurations
double InternalTemperature = 0;   // Start with something easily identifyable as not having been measured

// Global variables
TMP102 _temp_sensor (0x48); // Default address initialization
// When true, device sleeps and only takes measurements every ~5 minutes.
// When false, device takes intervals every ~10 seconds (or 5 minutes depending on config)
// and does not sleep. (primarily used for debugging).
bool _bLowPower = true; // enabled by default

// Device name.
// TODO: Expose a function to set device config including name, etc. so that we can update these devices from the cloud.
// Ultimately these settings should be from config and stored in EEPROM
const String _me ("TestCore");

STARTUP(System.enableFeature(FEATURE_RESET_INFO));

void setup()
    {
        // If the reset reason was panic, that's our fault. Enter safe mode so that we can flash a fix for it.
        // FIXME: Consider removing this from "production" builds - if safe mode prevents our code from running
        // and we're only occasionally crashing, that may be better.
        if (System.resetReason() == RESET_REASON_PANIC)
            {
                System.enterSafeMode();
            }

        // Configure the LED pin
        pinMode(PIN_boardLed,OUTPUT); // Our on-board LED is output as well
        // Turn the LED on
        digitalWrite(PIN_boardLed,HIGH);

        // Join the i2c network so we can configure the TMP102
        _temp_sensor.begin ();
        // set the Conversion Rate
        //0-3: 0:0.25Hz, 1:1Hz, 2:4Hz, 3:8Hz
        _temp_sensor.setConversionRate(3);
        _temp_sensor.sleep();	// Switch sensor to low power mode

        // Configure the Low Power pin
        pinMode(PIN_LowPowerEnable,INPUT_PULLUP); // Our on-board LED is output as well
        _bLowPower = digitalRead(PIN_LowPowerEnable) == HIGH;
        // Flash the LED if low power mode is enabled
        if (_bLowPower)
            {
            for (int i = 0; i < 10; ++i)
                {
                digitalWrite(PIN_boardLed, (i%2==0)?HIGH:LOW); delay(50);
                }
            }

        // Publish that we've rebooted
        Particle.publish(String::format("%s::LowPowerMode",_me.c_str()),String(_bLowPower),60,PUBLIC);
        Particle.publish (String::format("%s::LastResetReason",_me.c_str()),String(System.resetReason()),60,PUBLIC);
        Particle.variable ("InsideTemp", InternalTemperature);
    }

// Now for the loop.
void loop()
    {
        if (Particle.connected())
            {
                // Turn the on-board LED on while we;re processing.
                digitalWrite(PIN_boardLed, HIGH);
                // Turn sensor on to start temperature measurement.
                // Current consumtion typically ~10uA.
                _temp_sensor.wakeup();	// Wakeup and start running in normal power mode
                // Give the sensor time to wake up and do a couple of measurements before we ask for data.
                // With 8hz rate, reading every 125ms, 250 should be safe.
                delay(250);
                // Correction factor due to sensor possibly being old/having drifted?
                // Unable to use a consistent sensor correction factor. Drifing all over the place or am I using incorrectly?
                const auto actual_f = _temp_sensor.readTempF();
                const auto actual_c = _temp_sensor.readTempC();
                Particle.publish (String::format("%s::ReadTemps",_me.c_str()),
                                  String::format("F=%f,C=%f",actual_f,actual_c),
                                  60,PUBLIC);
                InternalTemperature = actual_f - 18;
                _temp_sensor.sleep();	// Switch sensor to low power mode

                Particle.publish("InternalTemperature", String(InternalTemperature),60,PUBLIC);

                // Make sure the LED stays on for at least 500ms
                delay(250);
                digitalWrite(PIN_boardLed, LOW);
            }
        // Depending on power mode config, delay or put the device to sleep for 4 minutes, 30 seconds.
        if (_bLowPower)
            {
            // Stay awake for another 30 seconds to round out to 5 minutes
            delay((28 * 1000) + 500);
            Particle.sleep (SLEEP_MODE_DEEP, ((60*4)+30));
            }
        else
            {
            // delay (((60*4)+30) * 1000);
            delay (10 * 1000 - 250);  // Take measurements ~ every 10 seconds
            }
    }
