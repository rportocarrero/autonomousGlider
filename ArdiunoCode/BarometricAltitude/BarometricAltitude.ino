/*
 *  This created a BLE peripheral with a service that
 *  contains a characteristic that outputs the current barometric altitude
 *  
 * 
 */
#include <Arduino_LPS22HB.h>
#include <ArduinoBLE.h>

BLEService altService("BarometricAltimeter"); // BLE Barometric Altitude Sensor

// BLE Barometric altitude Characteristic - custom 128-bit UUID, readable by central
BLEFloatCharacteristic pressureCharacteristic("Pressure", BLERead | BLENotify);
BLEFloatCharacteristic deltaPressureCharacteristic("DeltaPressure", BLERead | BLENotify);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);

  // begin barometer initialization
  if (!BARO.begin()){
    Serial.println("Failed to initialize pressure sensor!");
    while(1){
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected
  
  // begin BLE initialization
  if (!BLE.begin()){
      Serial.println("Starting BLE failed");
      while(1){
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }

  //set avertised local name and service UUID:
  BLE.setLocalName("Altimeter");
  BLE.setAdvertisedService(altService.uuid());

  // add the characteristic to the service
  altService.addCharacteristic(pressureCharacteristic);
  altService.addCharacteristic(deltaPressureCharacteristic);

  // add service
  BLE.addService(altService);

  // set the initial value for the characteristic:
   pressureCharacteristic.writeValue(0);
   deltaPressureCharacteristic.writeValue(0);

   // start advertising
   BLE.advertise();
   Serial.println("BLE Barometric Altitude Peripheral");
}

float P0 = 101.325; // Average sea level pressure (kPa)
float g = 9.807;    // Gravitational acceleration
float M = 0.02896;  // Molar mass of Earth's air (kg/mol)
float T = 288.15;  // Standard temperature (K)
float R = 8.3143; // Univeeral Gas Constant (N*m)/(mol*K) 
int updateDelay = 5000; // update every 5 seconds
unsigned long time_now = 0;

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if(central){
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    // read the sensor value
    float oldPressure = BARO.readPressure();
    float newPressure  = 0;
    double lastUpdate = millis();
    double delayStep = 1000;
    float deltaPressure = 0;
    
    // while the central is still connected to peripheral
    while (central.connected()){
      // write the barometric altitude to the characteristic
      if(millis() > lastUpdate + delayStep){
        lastUpdate = millis();
        if(newPressure = BARO.readPressure() != oldPressure){
          deltaPressure = newPressure - oldPressure;
          pressureCharacteristic.writeValue(newPressure);
          Serial.print("newPressure: ");
          Serial.println(newPressure);
          Serial.println(BARO.readPressure());
          deltaPressureCharacteristic.writeValue(deltaPressure);
          Serial.print("deltaPressure: ");
          Serial.println(deltaPressure);
        }
      }
    }
  }

    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
  
}
