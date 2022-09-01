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


//includes SD, SPI, and BME libraries
#include <SPI.h>  
#include <SD.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"


//define SPI pins
#define SCK 13
#define MISO 12
#define MOSI 11
#define BME_CS 3
#define SD_CS 4

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme(BME_CS, MOSI, MISO,  SCK);

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
      file = SD.open(dataFile, FILE_WRITE);
    
      // if the file is available, write to it:
      if (!file) {
        Serial.println("error opening datalog.txt");
      }
    }


    //writes to file SD Card opened
    void sdWrite (String dataString) {
      file.println(dataString);
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
      altimeterData = "Temperature: " + String(bme.temperature) + " *C, " + "Pressure: " + String(bme.pressure / 100.00) + " hPa, " + "Humidity: " + String(bme.humidity)
      + " %, " + "Gas: " + String(bme.gas_resistance / 100.0) + " KOhms, " + "Approx. Altitude: " + String(bme.readAltitude(SEALEVELPRESSURE_HPA)) + " m"; 
      return altimeterData;
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

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  while (!Serial);
  Serial.println(F("BME680 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 

  double sum;
  
  for (int i = 0; i <= 10; i++) {
    sum += bme.readAltitude(SEALEVELPRESSURE_HPA);
  }

  initAltitude = sum / 11;

  
}

SDCard sdcard;
Altimeter altimeter;

void loop() {

  sdcard.sdWrite(altimeter.toString());
  Serial.println(altimeter.toString());

}