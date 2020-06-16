/*
 modified 16 Jun 2020
 by Ryan Portocarrero
*/

#include <Servo.h>
#include <Arduino_LSM9DS1.h>

const float alpha = 0.5;
const int OFFSET = 90;
double fXg = 0;
double fYg = 0;
double fZg =0;


Servo lServo;  // create servo object to control a servo
Servo rServo;


void setup() {
  //Serial.begin(9600);
  
  lServo.attach(9);
  rServo.attach(6);
  
  lServo.write(OFFSET);
  rServo.write(OFFSET);
  if(!IMU.begin()){
    while(1);
  }
}

// calculates flight path angles from the accelerometer
void getFPA(float &p, float &r){
  float Xg, Yg, Zg, Xd, Yd, Zd;
  if(IMU.accelerationAvailable()){
    IMU.readAcceleration(Xg,Yg,Zg);
  }
  if(IMU.gyroscopeAvailable()){
    IMU.readGyroscope(Xd, Yd, Zd);
  }

  //Low Pass Filter
    //fXg = Xg * alpha + (fXg * (1.0 - alpha));
    //fYg = Yg * alpha + (fYg * (1.0 - alpha));
    //fZg = Zg * alpha + (fZg * (1.0 - alpha));
 
    //Roll & Pitch Equations
    r  = (atan2(-fYg, fZg)*180.0)/M_PI;
    p = (atan2(fXg, sqrt(fYg*fYg + fZg*fZg))*180.0)/M_PI;
}

void loop() {
    float pitch,roll;
    getFPA(pitch, roll);

    //Serial.print(pitch);
    //Serial.print("\t");
    //Serial.println(roll);

    lServo.write(OFFSET+pitch-roll);
    rServo.write(OFFSET-pitch-roll);
    
    delay(10);
  
}
