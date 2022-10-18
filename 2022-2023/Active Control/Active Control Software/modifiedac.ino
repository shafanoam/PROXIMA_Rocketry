/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define BME_SCK 7
#define BME_MISO 9
#define BME_MOSI 8
#define BME_CS 10
#define SD_CS 4

#define SEALEVELPRESSURE_HPA (1013.25)


Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

void setup() {

  pinMode(BME_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);

  digitalWrite(SD_CS, LOW);
  digitalWrite(BME_CS, LOW);
  
  // Open serial communications and wait for port to open:
  digitalWrite(BME_CS, HIGH);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println(F("BME680 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  bme.setPressureOversampling(BME680_OS_4X);

  digitalWrite(BME_CS, LOW);

  digitalWrite(SD_CS, HIGH);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  digitalWrite(SD_CS, LOW);

  
}

void loop() {

  digitalWrite(BME_CS, HIGH);
  
  // make a string for assembling the data to log:
  String dataString = String(bme.readAltitude(SEALEVELPRESSURE_HPA));

  digitalWrite(BME_CS, LOW);

  digitalWrite(SD_CS, HIGH);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening hello.txt");
  }

  digitalWrite(SD_CS, LOW);
}
