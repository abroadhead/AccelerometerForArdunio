//Credit to Kai for finding most of this off the interwebs
//Pi connected by two 100 resistors connected in parallel, for 3.05V

#include <Wire.h>

// Electrical pins
const int PI_PIN = 13; //Turns on the Camera

// Gyro definitions
long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

const int DELAY_TIME = 200; //delay between data mesuring (ms)

void setup() {
  Serial.begin(9600);
  
  Wire.begin();
  setupMPU(); //sets up GY-520 to collect data

  pinMode(PI_PIN, OUTPUT);
}

void loop() {
  recordAccelRegisters(); //get and process accleromter data
  
  if (gForceZ < 0.6) {  //
    startExperiment();
  }
  else
  {  //if false, do nothing
    Serial.println("Not currently falling.");
  }

  delay(DELAY_TIME);
}

//More confusing stuff is down here

void setupMPU() {
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
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
  Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print(" Z=");
  Serial.print(gForceZ);
  Serial.println("");
}

void startExperiment() {
  Serial.print("Starting experiment...");
  digitalWrite(PI_PIN, HIGH);
  delay(1000);
  Serial.println(" experiment running!");
  digitalWrite(PI_PIN, LOW);
}

