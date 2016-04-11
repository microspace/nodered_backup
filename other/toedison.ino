// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include <ArduinoJson.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>

#define DHTPIN 2     // what digital pin we're connected to
#include <SFE_BMP180.h>
SFE_BMP180 pressure;
#define ALTITUDE 180.0
int light = 0;
double T,P,p0,a;
char status;
#define DHTTYPE DHT11   // DHT 11
StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  root["pressure"] = 0;
  root["temperature"] = 0;
  root["humidity"] = 0;
  root["light"] = 0;  
  dht.begin();
  if (!pressure.begin())
  {
    //Serial.println("BMP180 init fail\n\n");
    root["pressure"] = 0;
    //while(1); // Pause forever.
  }
}

void loop() {
  // Wait a few seconds between measurements.
  delay(10000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    root["pressure"] = 0;
    //Serial.println("Failed to read from DHT sensor!");
    //return;
  }
 // Serial.print("Humidity: ");
 // Serial.print(h);
 // Serial.print(" %\t");
 // Serial.print("Temperature: ");
 // Serial.print(t);
 // Serial.print(" *C ");
root["humidity"] = h;

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      //Serial.print("temperature180: ");
      //Serial.print(T,2);
      //Serial.print(" deg C, ");
      root["temperature"] = T;
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.
      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);
        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          //Serial.print("absolute pressure: ");
          //Serial.print(P,2);
          //Serial.print(" mb, ");
          //Serial.print(P*0.0295333727,2);
          //Serial.println(" inHg");
          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb
          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          //Serial.print("relative (sea-level) pressure: ");
          //Serial.print(p0,2);
          root["pressure"] = p0;
          //Serial.print(" mb, ");
          //Serial.print(p0*0.0295333727,2);
          //Serial.println(" inHg");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          //a = pressure.altitude(P,p0);
          //Serial.print("computed altitude: ");
          //Serial.print(a,0);
          //Serial.print(" meters, ");
          //Serial.print(a*3.28084,0);
          //Serial.println(" feet");
        }
        //else Serial.println("error retrieving pressure measurement\n");
      }
      //else Serial.println("error starting pressure measurement\n");
    }
    //else Serial.println("error retrieving temperature measurement\n");
  }
  //else Serial.println("error starting temperature measurement\n");

light = analogRead(0);
root["light"] = light;

root.printTo(Serial);
Serial.println();






}
