#include <FuGPS.h>
#include <SPI.h>
#include <SD.h>
const boolean DEBUG = false;
const int chipSelect = 4;
const int TIMESTEP = 1000; // 1 second sample rate

FuGPS fuGPS(Serial1);
bool gpsAlive = false;

float alt, lat, lon, spd, crs, lastTimestep= -1;
String tme;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    if(DEBUG)Serial.begin(9600);
    Serial1.begin(9600);
    
    //fuGPS.sendCommand(FUGPS_PMTK_API_SET_NMEA_OUTPUT_RMCGGA);
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
    // don't do anything more:
  }
  if(DEBUG)Serial.println("card initialized.");
}

void error(String message){
  if(DEBUG){
    Serial.println(message);
  }
  while(1){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  
}

void loop()
{
    if(millis()-lastTimestep < TIMESTEP)return;
    if (fuGPS.read())
    {
        gpsAlive = true;

        if(DEBUG){
          Serial.print("Quality: ");
          Serial.println(fuGPS.Quality);

          Serial.print("Satellites: ");
          Serial.println(fuGPS.Satellites);
        }

        if (fuGPS.hasFix() == true)
        {
            digitalWrite(LED_BUILTIN, LOW); 
            // Data from GGA message
            //Serial.print("Accuracy (HDOP): ");
            //Serial.println(fuGPS.Accuracy);
            //tme = String(fuGPS.Hours,6) +"."+ String(fuGPS.Minutes,6) + "." + String(fuGPS.Seconds,6);
            lastTimestep = millis();

            //Serial.print("Altitude (above sea level): ");
            alt = fuGPS.Altitude;

            // Data from GGA or RMC
            //Serial.print("Location (decimal degrees): ");
            lat = fuGPS.Latitude;
            lon = fuGPS.Longitude;

            //Serial.print("speed: ");
            spd = fuGPS.Speed;

            //Serial.print("course: ");
            crs = fuGPS.Course;

            File dataFile = SD.open("GPSLOG.txt", FILE_WRITE);

            String dataString = String(millis());
            dataString += String(lat,6)+",";
            dataString += String(lon,6)+", ";
            dataString += String(alt,6)+", ";
            dataString += String(spd,6)+", ";
            dataString += String(crs,6);

            if(dataFile){
              dataFile.println(dataString);
              dataFile.close();
            }

            if(DEBUG){
              Serial.print(String(tme)+",");
            Serial.print(String(lat)+", ");
            Serial.print(String(lon)+", ");
            Serial.print(String(alt)+", ");
            Serial.print(String(spd)+", ");
            Serial.println(String(crs));
            }
           
        }
        else{
          digitalWrite(LED_BUILTIN, HIGH);
        }
    }

    // Default is 10 seconds
    if (fuGPS.isAlive() == false)
    {
        if (gpsAlive == true)
        {
            gpsAlive = false;
            error("GPS module not responding with valid data.");
            error("Check wiring or restart.");
        }
    }
}
