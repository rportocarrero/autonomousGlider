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
const float DECLINATION = 0 * DEG2RAD;  // magnetic declination in radians

float magErrx, magErry, magErrz; // magnetometer calibration errrors;
float max_x,min_x,max_y,min_y,max_z,min_z = 0;  // magnetometer calibration boundaries

float roll,pitch,heading = 0;

void setup() {
  if(DEBUG){
    Serial.begin(9600);
    while (!Serial);
    Serial.println(F("Started"));
  }

  if (!IMU.begin()) {
    if(DEBUG)Serial.println(F("Failed to initialize IMU!"));
    while (1);
  }

  if(DEBUG){
    Serial.println(F("Roll\tPitch"));
  }
}

// sensor fusion function
void getAttitude(float &p, float &r){
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
}

// check magnetometer calibration
void checkCalibration(float x, float y, float z){
  // recalculate boundary values
  if(x > max_x) max_x = x;
  if(x < min_x) min_x = x;
  if(y > max_y) max_y = y;
  if(y < min_y) min_y = y;
  if(z > max_z) max_z = z;
  if(z < min_z) min_z = z;

  //establish offsets
  magErrx = (max_x + min_x) * 0.5;
  magErry = (max_y + min_y) * 0.5;
  magErrz = (max_z + min_z) * 0.5;
}

// get the magnetic heading from the magnetometer
void getHeading(float &h){
  float x, y, z;
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(x, y, z);
    checkCalibration(x,y,z);
    h = atan2(-(y-magErry), -(x-magErrx));
    if(h<0) h+= 2*M_PI;
    h += DECLINATION;
  }
}

// Main Loop
void loop() {
  int startTime = millis();

  // read attitude and heading
  getAttitude(pitch, roll);
  getHeading(heading);

  // display attitude and heading
   if(DEBUG){
      Serial.print(pitch*RAD2DEG);
      Serial.print(F(",\t"));
      Serial.print(roll*RAD2DEG);
      Serial.print(F(",\t"));
      Serial.println(heading*RAD2DEG);
   }
  
  int endTime = millis();
  while(endTime - startTime < TIMESTEP){
    endTime = millis();
  }
}
