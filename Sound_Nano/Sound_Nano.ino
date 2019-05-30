//Credit to Kai for finding most of this off the interwebs
//Pi connected by two 100 resistors connected in parallel, for 3.05V

#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// Gyro definitions
long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

int tonecounter = 0;
int tonevalue = 400;

File datafile;

void setup() {
  Serial.begin(9600);
  
  Wire.begin();
  
  if (!SD.begin(10)) { //Tests if CS pin could find an SD card
    Serial.println("init failed");
  } else {
    Serial.println("init successful");
  }
 
  datafile = SD.open("dataone.txt", FILE_WRITE);

  datafile.println(" ");
  datafile.println("----RESET----");
  datafile.println(" ");

  datafile.close();

  setupMPU(); //sets up GY-520 to collect data
}

void loop() {
  recordAccelRegisters(); //get and process accleromter data

  if (gForceZ < 0.6) {
    datafile = SD.open("dataone.txt", FILE_WRITE);
    datafile.println("-------------");
    datafile.println("---FALLING---");
    datafile.println("-------------");
    datafile.close();
  }
  
  printData(); //print data to serial and SD
  
  tonecounter = ++tonecounter;
  Serial.print(tonecounter);
  Serial.println(" = count");
  
  if (tonecounter < 16){
    tonevalue = 25;
    Serial.println(tonevalue);
  }
  if (tonecounter < 11){
    tonevalue = 200;
    Serial.println(tonevalue);
  }
  if (tonecounter < 6){
    tonevalue = 400;
    Serial.println(tonevalue);
  }
  if (tonecounter == 15){
    tonecounter = 0;
  }
    
  tone(8, tonevalue, 5000); //play tone @ 400hz (port, hz, time)
  
  delay(1000);
}

//More confusing stuff is down here

void setupMPU() {
  Wire.beginTransmission(0b1101000); //This is the I2C addre  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
//ss of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  gForceX = accelX / 16384.0; //Convert the data into g-forces
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}

void printData() {
  datafile = SD.open("dataone.txt", FILE_WRITE);

  Serial.print(gForceX); //display data on serial logger
  Serial.print(", ");
  Serial.print(gForceY);
  Serial.print(", ");
  Serial.println(gForceZ);
  
  datafile.print(gForceX); //write data to SD card
  datafile.print(", ");
  datafile.print(gForceY);
  datafile.print(", ");
  datafile.println(gForceZ);

  datafile.close();
}
