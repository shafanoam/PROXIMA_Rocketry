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
#include <Servo.h>

#define BME_SCK 7
#define BME_MISO 9
#define BME_MOSI 8
#define BME_CS 10
#define SD_CS 3

#define SEALEVELPRESSURE_HPA (1013.25)


Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);
Servo myservo;

double initAltitude = 0;

class Altimeter {

    //altimeterData String, changes with each usage of toString()
  private:

    String altimeterData;

  public:

    //altimeter constructor, recognizes if altimeter has failed

    Altimeter () {
      digitalWrite(BME_CS, HIGH);

      if (! bme.performReading()) {
        Serial.println("Failed to perform reading :(");
      }

      digitalWrite(BME_CS, LOW);
    }

    //puts all altimeter data into altimeterData each time it is called
    String toString () {
      digitalWrite(BME_CS, HIGH);

      //return ("Temperature: " + String(bme.temperature) + " *C, " + "Pressure: " + String(bme.pressure / 100.00) + " hPa, " + "Humidity: " + String(bme.humidity) + " %, " + "Gas: " + String(bme.gas_resistance / 100.0) + " KOhms, " + "Approx. Altitude: " + String(bme.readAltitude(SEALEVELPRESSURE_HPA)) + " m");
      return String(bme.readAltitude(SEALEVELPRESSURE_HPA));
    }


    //returns the altitude above sea level
    double getAbsoluteAltitude () {
      digitalWrite(BME_CS, HIGH);

      return bme.readAltitude(SEALEVELPRESSURE_HPA);

      digitalWrite(BME_CS, LOW);
    }


    //returns the altitude change from initialization
    double getRelativeAltitude () {
      digitalWrite(BME_CS, HIGH);

      return bme.readAltitude(SEALEVELPRESSURE_HPA) - initAltitude;

      digitalWrite(BME_CS, LOW);
    }
};

Altimeter altimeter;

void setup() {

  pinMode(BME_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

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

  bme.setPressureOversampling(BME680_OS_8X);
  bme.setGasHeater(25, 1);

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

  digitalWrite(BME_CS, HIGH);

  int sum = 0;
  
  for (int i = 0; i <= 10; i++) {
    sum += bme.readAltitude(SEALEVELPRESSURE_HPA);
  }

  initAltitude = sum / 11;

  digitalWrite(BME_CS, LOW);

  digitalWrite(SD_CS, LOW);
  
  myservo.attach(2);

  
}

int timer;
int h = 0;
double alt = 0;

void loop() {

  digitalWrite(BME_CS, HIGH);
  
  // make a string for assembling the data to log:
  alt = altimeter.getRelativeAltitude();
  String dataString = String(alt);
  
  digitalWrite(BME_CS, LOW);

  digitalWrite(SD_CS, HIGH);

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog1.txt", FILE_WRITE);
 
 // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
  } else {

//    while (true) {
//      delay(1000);
//    }
  }
 

  digitalWrite(SD_CS, LOW);


  if (alt >= 1) {

    myservo.write(180);
    
    if (h == 0) {
      timer = millis();
      h++;
    }
  }
  if (millis() - timer >= 2500) {

    myservo.write(90);
    
  }
}
