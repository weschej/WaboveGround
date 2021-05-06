/*
  
  Wabove Ground
  Written by: John Wesche

  This program reads data from the BMP280 and HTU21D-F sensors and outputs
  the readings to a microSD card where it can later be converted
  into a spreadsheet for analysis

  v1.3
  Created: 4/26/21
  Revised: 5/4/21
  
*/

// Libraries to include
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_HTU21DF.h>

// Creating a file to write data to
File myFile;

// Initializing variables
int num = 1;
int switchPin = 7;
float temp = 0;
float pressure = 0;
float rel_hum = 0;
const int chipSelect = 10;


// Initializing sensors and SD card
Adafruit_BMP280 bmp;
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

void setup() {
  pinMode(switchPin, INPUT_PULLUP);
  
  if (!bmp.begin()) {
    while(1);
  }

  if (!htu.begin()) {
    while(1);
  }

  if(!SD.begin(10)) {
    while(1);
  }

  SD.remove("DataLog.txt");                           // Removing old file to prevent overwrite

  myFile = SD.open("DataLog.txt", FILE_WRITE);        // Opening file to create header

  myFile.print("Reading Number, Temperature (*C), Pressure (Pa), Rel. Humidity (%)"); // Create headings
  myFile.println();
  
  myFile.close();
  // Setting up the BMP280
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,       // Operating Mode
                    Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling
                    Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
                    Adafruit_BMP280::FILTER_X16,      // Filtering
                    Adafruit_BMP280::STANDBY_MS_500); // Standby time
}

void loop() {
  myFile = SD.open("DataLog.txt", FILE_WRITE);        // Open file to write data too

  if(digitalRead(7) == LOW) {                         // If switch is in "on" position
    temp = bmp.readTemperature();                     // Take readings and store to correseponding variables
    pressure = bmp.readPressure();
    rel_hum = htu.readHumidity();

    myFile.print(num);                                // Prints the reading number to correlate to a time stamp
    myFile.print(",");
    
    myFile.print(temp);
    myFile.print(",");
        
    myFile.print(pressure);
    myFile.print(",");
    
    myFile.println(rel_hum);

    num = num + 1;                                    // Simple counter 
  }
  
  myFile.close();                                     // Close file to remove card 
    
  delay(1000);                                        // Sampling interval
  
}
