# WaboveGround
WaboveGround in its simplest form, is a compact sensor array used for collecting meteorological data and storing that data to a file on a microSD card.

<img src="https://user-images.githubusercontent.com/83726389/117233123-80d2ff80-adf0-11eb-8978-40fb2ffe193a.jpg" width="480">

# Summary
The goal of this project is to utilize a combination of electronics and sensors to solve a problem. Our team developed a sensor pack capable of measuring and recording temperature, pressure, and humidity data that could be coupled with an autonomous vehicle for the purpose of agricultural or environmental research. The project had three main goals: create code able to import and store data, develop a housing system to mount the sensors to a drone, and construct a system capable of accurately gathering environmental data. After testing each component separately as well as a whole, we were able to successfully gather data as outlined in our project goals.

# Design Description
## Supplies
* Arduino Uno
* Adafruit BMP280
* Adafruit HTU21D-F
* Adafruit Micro SD Card Breakout Board

## The Circuit
![Circuit](https://user-images.githubusercontent.com/83726389/117231144-9e05cf00-adec-11eb-99ce-af9eed779261.png)
While this diagram shows the use of a breadboard, this is just for simplicity. All connections should be soldered and heat shrink should be used.

5V power is needed for both sensors and the breakout board. In our initial development, this was accomplished using a micro USB cable. While integrating into the drone we used the drone voltage output to power the Arduino using a barrel jack. This jack has a regulator that takes the ~12V of input voltage and brings it down to the preferred 5V. 

A simple toggle slide switch was used to initialize and halt the data logging process. Digital pin 7 was used but any unused digital pin could be used instead with some minor code modification. The center pin of the switch is connected to ground while one of the outer terminals is connected to the digital pin.

The BMP280 and HTU21D-F communicate using the I2C interface. They need 4 connections to function properly; 5V, ground, clock, and data. 5V and ground are pulled directly from the corresponding pins on the Arduino. The clock connection is noted on the sensor by either "SCK" or "SCL" these terminals should be connected to the A5 pin on an Arduino Uno. The final connection for the sensors will be data. This is denoted with either "SDA" or "SDI" and should be attached to the A4 pin. 

The microSD board requires 6 connections and will use hardware SPI. 5V and ground should be wired into their matching terminals. Digital pins 10-13 are the hardware SPI pins. Pin 10 is the chip select (CS) pin, 11 is the data in (DI) pin, 12 is the data out (DO) pin, and 13 is the clock (CLK) pin.

## The Case
An enclosure can be 3D printed to house the Arduino and microSD breakout boards and the top can also be used to fasten the sensors. The case is improved by using some additional hardware, namely nut inserts to screw the top down and hold the Arduino in the bottom. 

<img src="https://user-images.githubusercontent.com/83726389/117231182-b2e26280-adec-11eb-97f8-6e011e12a981.png" width="480">

## The Code
```c
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
```

# Testing Description
Preliminary testing consisted of testing the sensors’ ability to gather and store data as executed by the code. After testing the ability of the sensor’s to gather data manually, the sensor array was attached to the drone to test the system as a whole. The drone flew to a designated location, gathered environmental data, and flew back. The data was then imported into Excel for analysis.
## Test Equipment
* Laptop or PC
* Excel or similar spreadsheet software
## Procedure
1. Ensure the microSD card is inserted into the breakout board
1. Slide switch should be in the "off" position unless ready to record data
1. Upload the code onto the Arduino
1. When ready to begin the logging process, slide the switch to the "on" position
1. After recording for desired time, slide switch to "off" 
1. Remove the microSD card from the breakout board and connect to PC or laptop
1. Open spreadsheet software and in the software open "DATALOG.txt"
1. If prompted, select "comma" as the delimiter
1. Verify results 

# Design Decision Discussion
The two sensors used for this project were chosen based on their uses and how they support our project goals. The BMP 280 sensor was picked as it measures pressure and temperature, which allows for monitoring of weather patterns. Additionally, the HTU21D-F measures humidity, which is also beneficial when gathering environmental data. All three sensors are also important as they could detect changes in temperature, pressure, and humidity and warn of inclement weather. The drone itself is capable of measuring altitude, which can help determine the drone’s positioning and location. 

Due to barometric pressure changing as altitude changes, the BMP280 can estimate the altitude of itself. The problem with using this method is that it relies on updating the code to include the barometric pressure of the testing location in hPa at sea level. The manufacturer estimates that if the pressure is not updated per location, the reading will only be accurate to within 10 meters. The drone's GPS based altitude provided more accurate results when compared to the BMP280.

# Test Results Discussion
This section will analyze the findings from this endeavor along with the limits of the sensors and where the system would work best.

## Sensor Limitations
### BMP280
> Pressure Range: 300 to 1100hPa

> Temperature Range: -40 to +85°C

### HTU21D-F
> Humidity Range: 0 to 100%RH

### microSD Card Breakout Board
> Durability: 10,000 insertion/removal cycles

These components will work in any normal operating conditions and should be reliable. One of the main concerns when dealing with any electronics is dust prevention. This system does not currently have a method of controlling dust buildup, so a clean environment would increase the longevity of this product. 

This system functioned without failure throughout all of the trials our team put it through. It reliably logged temperature, pressure, and relative humidity to the storage device countless times. 
