#include <Wire.h> 
#include <Adafruit_Sensor.h> 
#include <Adafruit_BMP280.h>

// I2C Interface
Adafruit_BMP280 bme;

void setup() {
  Serial.begin(9600);

  if (!bme.begin(0x76)) { 
    Serial.println("Error! No BMP Sensor Detected!!!");
    while (1);
  }
}

void loop() {
  Serial.print("------------BMP 280 -------------\n");
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100); // 100 Pa = 1 millibar
  Serial.println(" mb");
  Serial.print("Approx Altitude = ");
  Serial.print(bme.readAltitude(1013.25)); // Atmospheric pressure at sea level
  Serial.println(" m");
  Serial.print("--------------------------------\n\n");

  delay(2000);
}
