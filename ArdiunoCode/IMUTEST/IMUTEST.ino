/*
 * This file takes a 10s sample of stationary gyroscope data for calibration analysis
 */

#include <Arduino_LSM9DS1.h>
const float G2MS = -9.8;
const int TIMESTEP = 10; // limits sample rate of IMU
float lastSampleTime;
float startTime;

void setup() {
  Serial.begin(9600);
  while(!Serial);

  if(!IMU.begin()){
    Serial.println("Failed to initialize IMU!");
    while(1);
  }
  Serial.println("wX,wY,wZ,aX,aY,aZ");
  delay(5000); // wait for gyro/accel to settle
  startTime = millis();
}

void loop() {
  float wX,wY,wZ,aX,aY,aZ;
  if(millis()-startTime > 10000){
    return;
  }
  if(millis()-lastSampleTime < TIMESTEP){
    return;
  }
  if(IMU.gyroscopeAvailable()&&IMU.accelerationAvailable()){
    IMU.readAcceleration(aX,aY,aZ);
    IMU.readGyroscope(wX,wY,wZ);
    lastSampleTime = millis();
  Serial.print(wX,7);
  Serial.print(",");
  Serial.print(wY,7);
  Serial.print(",");
  Serial.print(wZ,7);
  Serial.print(",");
  Serial.print(aX,7);
  Serial.print(",");
  Serial.print(aY,7);
  Serial.print(",");
  Serial.println(aZ,7);
    
  }
}
