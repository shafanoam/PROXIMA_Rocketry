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
 ** BME CS - pin 10
 ** SD CS - pin 4

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/



#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <SD.h>
#include <Servo.h>

#define BME_SCK 5
#define BME_SDO 6
#define BME_SDI 7
#define BME_CS 10
#define SD_CS 4

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme(BME_CS, BME_SDI, BME_SDO,  BME_SCK);

int pos = 0;
double initAltitude = 0;
File root;

int fileCountOnSD = 0; 

class SDCard {
  private:
    
    String dataFile;
    File file;
  
  public:
    
    //TO DO: implement way to create numbered files automatically 
    SDCard () {

      while (true) {

        File entry =  root.openNextFile();
        if (!entry) {
          // no more files
          break;
        }
        
        // for each file count it
        fileCountOnSD++;
    
        entry.close();
      }

      fileCountOnSD += 1;

      dataFile = String(fileCountOnSD) + ".txt";
      
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
      File file = SD.open("hello.txt", FILE_WRITE);
    
      // if the file is available, write to it:
      if (!file) {
        Serial.println("error opening datalog.txt");
      }
    }


    //writes to file SD Card opened
    void sdWrite (String dataString) {
      File file = SD.open("hello.txt", FILE_WRITE);
      
      if (file) {
        file.println(dataString);
        file.close();
        // print to the serial port too:
        Serial.println(dataString);
      }
      // if the file isn't open, pop up an error:
      else {
        Serial.println("error opening datalog.txt");
      }
    }

    //closes file SD Card was writing to
    void closeFile () {
        file.close();
    }
};

class Altimeter {

  //altimeterData String, changes with each usage of toString()
  private:
    
    String altimeterData;

  public:

    //altimeter constructor, recognizes if altimeter has failed
    Altimeter () {
      if (! bme.performReading()) {
        Serial.println("Failed to perform reading :(");
      }  
    }

    //puts all altimeter data into altimeterData each time it is called
    String toString () {
      //return ("Temperature: " + String(bme.temperature) + " *C, " + "Pressure: " + String(bme.pressure / 100.00) + " hPa, " + "Humidity: " + String(bme.humidity) + " %, " + "Gas: " + String(bme.gas_resistance / 100.0) + " KOhms, " + "Approx. Altitude: " + String(bme.readAltitude(SEALEVELPRESSURE_HPA)) + " m"); 
      return String(bme.readAltitude(SEALEVELPRESSURE_HPA));
    }


    //returns the altitude above sea level
    double getAbsoluteAltitude () {
      return bme.readAltitude(SEALEVELPRESSURE_HPA);
    }


    //returns the altitude change from initialization
    double getRelativeAltitude () {
      return bme.readAltitude(SEALEVELPRESSURE_HPA) - initAltitude;
    }
};

Servo myservo;

void sweep () {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}

SDCard sdcard;
Altimeter altimeter;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("BME680 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setPressureOversampling(BME680_OS_4X);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  double sum;
  
  for (int i = 0; i <= 50; i++) {
    sum += bme.readAltitude(SEALEVELPRESSURE_HPA);
  }

  initAltitude = sum / 51;

  myservo.attach(9);
  
}

void loop() {
//  sdcard.sdWrite(altimeter.toString());
//  sdcard.closeFile();
//  Serial.println(altimeter.toString());
  Serial.println("Abs: " + String(altimeter.getAbsoluteAltitude()));
  Serial.println("Rel: " + String(altimeter.getRelativeAltitude()));

  if(altimeter.getRelativeAltitude() > 0.25) {
    sweep();
  }
}