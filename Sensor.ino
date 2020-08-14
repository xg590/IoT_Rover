/* 
sudo ln -s /home/pi/arduino-1.8.12/arduino /usr/local/bin/arduino
arduino --install-library "Adafruit Unified Sensor"
arduino --install-library "Adafruit BNO055" 
arduino --board arduino:avr:pro --port /dev/ttyUSB0 --upload Sensor.ino

python 
import serial
with serial.Serial(port='/dev/ttyUSB0', baudrate=115200, timeout=2) as s:
    while 1: print(s.readline())
*/
#include <SoftwareSerial.h>  
#include <TinyGPS.h>    
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Wire.h>

TinyGPS gps;
SoftwareSerial gps_uart(11, 10); // (Rx, Tx) 
SoftwareSerial report_uart(13, 12); // (Rx, Tx)  
Adafruit_BNO055 i2c = Adafruit_BNO055(55, 0x29); 

void setup(void) {
  report_uart.begin(9600); 
  gps_uart.begin(9600); // gps   
  i2c.begin();          // compass
  delay(1000);          // delay for compass 
}

void loop() {   
  gy_bno055(); 
  gy_neo6mv2();   
  voltmeter(A0);
  gps_update(1000);  
} 
 
static void gy_bno055() {
  sensors_event_t bno;
  i2c.getEvent(&bno, Adafruit_BNO055::VECTOR_EULER); 
  report_uart.print(" T|") ; report_uart.print(i2c.getTemp());
  report_uart.print(" x|") ; report_uart.print(bno.orientation.x);
  report_uart.print(" y|") ; report_uart.print(bno.orientation.y);
  report_uart.print(" z|") ; report_uart.print(bno.orientation.z); 
} 

static void voltmeter(int pinId) {
  int sensorValue = analogRead(pinId);
  float voltage = sensorValue / 1023.0 * 5.0;  
  report_uart.print(" v0|") ; report_uart.print(voltage, 2);
}

static void gy_neo6mv2() { 
  float lat, lon; unsigned long _; 
  gps.f_get_position(&lat, &lon, &_); 
  if (lat                != TinyGPS::GPS_INVALID_F_ANGLE   ) { report_uart.print(" lat|") ; report_uart.print(lat, 6             );}
  if (lon                != TinyGPS::GPS_INVALID_F_ANGLE   ) { report_uart.print(" lon|") ; report_uart.print(lon, 6             );}
  if (gps.f_altitude()   != TinyGPS::GPS_INVALID_F_ALTITUDE) { report_uart.print(" alt|") ; report_uart.print(gps.f_altitude()   );}
  if (gps.f_course()     != TinyGPS::GPS_INVALID_F_ANGLE   ) { report_uart.print(" cus|") ; report_uart.print(gps.f_course()     );}
  if (gps.f_speed_kmph() != TinyGPS::GPS_INVALID_F_SPEED   ) { report_uart.print(" spd|") ; report_uart.print(gps.f_speed_kmph() );}
  if (gps.satellites()   != TinyGPS::GPS_INVALID_SATELLITES) { report_uart.print(" sat|") ; report_uart.print(gps.satellites()   );} 
  print_date(gps);
  report_uart.println(); 
} 
 
static void gps_update(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gps_uart.available())
      gps.encode(gps_uart.read());
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
    sprintf(sz, " ts|%02d-%02d-%02dT%02d:%02d:%02d gps|ok",
        year, month, day, hour, minute, second); 
    report_uart.print(sz);
  } else { report_uart.print(" gps|"); } // print nothing if not gps update
} 
