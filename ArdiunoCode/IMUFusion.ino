/*
  The circuit:
  - Arduino Nano 33 BLE Sense

  created 15 Jun 2020
  by Ryan Portocarrero

  Gyroscope sample rate = 119 hz (library says 104 hz)
  Accelerometer sample rate = 119 hz (library says 104 hz)
*/

#include <Arduino_LSM9DS1.h>

const boolean DEBUG = true; //flag for serial output
const int TIMESTEP = 10; // loop time in milliseconds
const float LPF_TUNER = .90; // tuning value for complementary filter
const float LPF_COMP = 1-LPF_TUNER; //complement of the above value
const float SEC2MIL = .001; // conversion factor from seconds to milliseconds
const float RAD2DEG = 180/M_PI; // conversion factor from radians to degrees
const float DEG2RAD = M_PI/180;

float roll,pitch = 0;

void setup() {
  if(DEBUG){
    Serial.begin(9600);
    while (!Serial);
    Serial.println("Started");
  }

  if (!IMU.begin()) {
    if(DEBUG)Serial.println("Failed to initialize IMU!");
    while (1);
  }

  if(DEBUG){
    Serial.println("Roll\tPitch\tProfile");
  }
}

// sensor fusion function
void getFPA(float &p, float &r){
  float aX, aY, aZ, gX, gY, gZ, accelRoll, accelPitch, gyroRoll, gyroPitch;
  if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable()) {
    IMU.readGyroscope(gX, gY, gZ);
    IMU.readAcceleration(aX,aY,aZ);
    
    //integrate gyroscope
    gyroRoll = r+gX*SEC2MIL*DEG2RAD;
    gyroPitch = p+gY*SEC2MIL*DEG2RAD;
    // z = z+dZ*TIMESTEP; gyro yaw
  
    // convert accelerometer values to angles
    accelRoll = atan2(-aY, aZ);
    accelPitch = atan2(aX, sqrt(aY*aY + aZ*aZ));
  
    // complement the predicted angles 
    r = LPF_TUNER * gyroRoll + LPF_COMP * accelRoll;
    p = LPF_TUNER * gyroPitch + LPF_COMP * accelPitch;
  }
  
  if(DEBUG){
    Serial.print(r*RAD2DEG);
    Serial.print("\t");
    Serial.print(p*RAD2DEG);
    Serial.print("\t");
  }
  
}

// Main Loop
void loop() {
  int startTime = millis();

  getFPA(pitch, roll);
  
  int endTime = millis();
  if(DEBUG)Serial.println(endTime-startTime);
  while(endTime - startTime < TIMESTEP){
    endTime = millis();
  }
}
