// -----------------------------------------
// Copyright 2018 Ian Maines
// All Rights Reserved
// -----------------------------------------
// This app generates bogus temperature and humidity data for testing various integrations
// (ie AWS IoT, IFTTT)

#include <vector>

#include <Wire.h>
#include "SparkFun_Si7021_Breakout_Library.h"

// Pin Configurations
int PIN_boardLed = D7; // On-board LED
// Use D3 as low power enable mode. Set to ground to disable.
// The goal is to not put the device to sleep when disabled so that firmware updates may be flashed.
int PIN_LowPowerEnable = D3;

// Exposed variable Configurations
double InternalTemperature = 0;   // Start with something easily identifyable as not having been measured
double InternalHumidity = 0;

// Global variables
//Create Instance of HTU21D or SI7021 temp and humidity sensor and MPL3115A2 barrometric sensor
Weather _sensor;
bool _bLowPower = true; // enabled by default

// Device name.
// TODO: Expose a function to set device config including name, etc. so that we can update these devices from the cloud.
// Ultimately these settings should be from config and stored in EEPROM
const String _me ("TestCore");
const String _ver ("0.1.1");

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



        //Initialize the I2C sensors and ping them
        if (!_sensor.begin())
            {

            }
        // Disable the heater
        _sensor.heaterOff();

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
        Particle.publish (String::format("%s::LastResetReason",_me.c_str()),String(System.resetReason()),60,PUBLIC);
        Particle.publish(String::format("%s::LowPowerMode",_me.c_str()),String(_bLowPower),60,PUBLIC);
        Particle.publish(String::format("%s::FirmwareVer",_me.c_str()), _ver.c_str(),60,PUBLIC);
        Particle.variable ("InsideTemp", InternalTemperature);
        Particle.variable ("InsideHumid", InternalHumidity);
    }

// Now for the loop.
void loop()
    {
        if (Particle.connected())
            {
                // Turn the on-board LED on while we;re processing.
                digitalWrite(PIN_boardLed, HIGH);

                // Measure Relative Humidity from the HTU21D or Si7021
                InternalHumidity = _sensor.getRH() + 2;
                // Measure Temperature from the HTU21D or Si7021
          
                InternalTemperature = _sensor.readTempF() -7.25;

                Particle.publish(String::format("%s::InternalTemperature", _me.c_str()), String(InternalTemperature),60,PUBLIC);
                Particle.publish(String::format("%s::InternalHumidity", _me.c_str()), String(InternalHumidity),60,PUBLIC);

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
