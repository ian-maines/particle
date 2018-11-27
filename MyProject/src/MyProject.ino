// -----------------------------------------
// Copyright 2018 Ian Maines
// All Rights Reserved
// -----------------------------------------
// This app generates bogus temperature and humidity data for testing various integrations
// (ie AWS IoT, IFTTT)


int PIN_boardLed = D7; // On-board LED
double InternalTemperature = 68;

// We start with the setup function.

void setup()
  {
  // Configure the LED pin
  pinMode(PIN_boardLed,OUTPUT); // Our on-board LED is output as well

  // Turn the LED off
  digitalWrite(PIN_boardLed,HIGH);

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
  InternalTemperature = posneg ? InternalTemperature + shift : InternalTemperature - shift;
  Particle.publish("InternalTemperature", String(InternalTemperature),60,PUBLIC);
  }
