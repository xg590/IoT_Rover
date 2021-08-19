#include <SoftwareSerial.h> 
#include <TinyGPS++.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
TinyGPSPlus gps; // create gps object 

Adafruit_BNO055 compass_i2c = Adafruit_BNO055(55, 0x29); // wire A4/A5 to SDA/SCL.
const int GPS_Rx = D7, GPS_Tx = D6, Vol_Mo = A0;
const int reportInterval = 1000; // millisecond
SoftwareSerial  gps_uart(GPS_Rx,  GPS_Tx); 

void setup(){ 
    Serial.begin(115200); delay(1000); while (!Serial); // connect serial  
    gps_uart.begin(9600); // connect gps sensor   
    compass_i2c.begin();  // Compass  
} 
 
unsigned long lastReportTime = millis();
void loop(){   
  Serial.print("{");
  sensors_event_t bno;
  compass_i2c.getEvent(&bno, Adafruit_BNO055::VECTOR_EULER);
  uint8_t sys, gyro, accel, mag = 0;
  compass_i2c.getCalibration(&sys, &gyro, &accel, &mag);
  
  Serial.print("\"Mg\":") ; Serial.print(mag);
  Serial.print(",\"T\":") ; Serial.print(compass_i2c.getTemp());
  Serial.print(",\"x\":") ; Serial.print(bno.orientation.x);
  Serial.print(",\"y\":") ; Serial.print(bno.orientation.y);
  Serial.print(",\"z\":") ; Serial.print(bno.orientation.z);
  while (gps_uart.available()) { 
    int r = gps_uart.read();
    gps.encode(r); 
  }
  if (millis() - lastReportTime > reportInterval) {
    lastReportTime = millis(); 
    char msg[128] = ",\"Sat\":0";
    if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid()) { 
      sprintf(msg,
              ",\"Sat\":%d,\"Lat\":%.6f,\"Lng\":%.6f,\"TS\":\"%02d-%02d-%02dT%02d:%02d:%02dZ\"",
              gps.satellites.value(), gps.location.lat(), gps.location.lng(),
              gps.date.year(), gps.date.month(), gps.date.day(),
              gps.time.hour(), gps.time.minute(), gps.time.second() // gps.time.centisecond()
             ); 
    } else { 
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
    } 
    Serial.print(msg);
    Serial.print(",\"V\":") ; Serial.print(analogRead(Vol_Mo)) ;
  } 
  Serial.println("}"); 
  delay(100);
}   
