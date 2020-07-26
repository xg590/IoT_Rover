#include <SoftwareSerial.h>  
#include <TinyGPS.h>  
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h> 
#include <Wire.h>

TinyGPS gps;
SoftwareSerial uart(0, 1); // (Rx, Tx)
Adafruit_BNO055 i2c = Adafruit_BNO055(55, 0x29); // id, address

void setup(void) {
  Serial.begin(115200); 
  uart.begin(9600);     // gps 
  i2c.begin();          // compass  
  delay(1000);          // delay for compass 
}

void loop() {  
  gy_bno055();  
  gy_neo6mv2();  
  Serial.println(); 
  gps_update(1000);  
} 

static void gy_bno055() {
  sensors_event_t bno;
  i2c.getEvent(&bno, Adafruit_BNO055::VECTOR_EULER); 
  Serial.print(" x|");   Serial.print(bno.orientation.x); 
  Serial.print(" y|");   Serial.print(bno.orientation.y); 
  Serial.print(" z|");   Serial.print(bno.orientation.z); 
  Serial.print(" cel|"); Serial.print(i2c.getTemp()    ); 
}   

static void gy_neo6mv2() { 
  float lat, lon; unsigned long _; 
  gps.f_get_position(&lat, &lon, &_); 
  if (lat                != TinyGPS::GPS_INVALID_F_ANGLE   ) { Serial.print(" lat|") ; Serial.print(lat, 6             );}
  if (lon                != TinyGPS::GPS_INVALID_F_ANGLE   ) { Serial.print(" lon|") ; Serial.print(lon, 6             );}
  if (gps.f_altitude()   != TinyGPS::GPS_INVALID_F_ALTITUDE) { Serial.print(" alt|") ; Serial.print(gps.f_altitude()   );}
  if (gps.f_course()     != TinyGPS::GPS_INVALID_F_ANGLE   ) { Serial.print(" cus|") ; Serial.print(gps.f_course()     );}
  if (gps.f_speed_kmph() != TinyGPS::GPS_INVALID_F_SPEED   ) { Serial.print(" spd|") ; Serial.print(gps.f_speed_kmph() );}
  if (gps.satellites()   != TinyGPS::GPS_INVALID_SATELLITES) { Serial.print(" sat|") ; Serial.print(gps.satellites()   );} 
  print_date(gps);
  Serial.println(); 
} 
 
static void gps_update(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (uart.available())
      gps.encode(uart.read());
  } while (millis() - start < ms);
} 

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age; // A varible defined by Library author
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age != TinyGPS::GPS_INVALID_AGE) {
    char sz[32];
    sprintf(sz, " ts|%02d-%02d-%02dT%02d:%02d:%02d",
        year, month, day, hour, minute, second); 
    Serial.print(sz);
  }  
} 
