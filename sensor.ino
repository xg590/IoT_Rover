#include <SoftwareSerial.h> 
#include <TinyGPS.h> 
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

TinyGPS tinygps;
SoftwareSerial uart(0, 1);
Adafruit_BNO055 i2c = Adafruit_BNO055(55, 0x29); 

void setup(void) {
  Serial.begin(115200);
  uart.begin(9600); // gps
  i2c.begin(); // compass
  delay(1000);
}

void loop() { 
  gy_neo6mv2(); 
  gy_bno055();
  voltmeter();
  delay(1000);
}

static void gy_neo6mv2() {
  while (uart.available()) {
    char c = uart.read(); // Serial.write(c);
    tinygps.encode(c); 
  }
  float flat, flon;
  unsigned long age = 0; 
  
  tinygps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 7);
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 7);
  print_float(tinygps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 1);
  print_float(tinygps.f_course(), TinyGPS::GPS_INVALID_F_ANGLE, 2);
  print_float(tinygps.f_speed_knots(), TinyGPS::GPS_INVALID_F_SPEED, 7);
  print_int(tinygps.satellites(), TinyGPS::GPS_INVALID_SATELLITES); 
  print_date(tinygps);
} 

static void print_int(unsigned long val, unsigned long invalid) { 
  if (val != invalid)
    Serial.print(val); 
  Serial.print(','); 
}

static void print_float(float val, float invalid, int prec) { 
  if (val != invalid) 
    Serial.print(val, prec); 
  Serial.print(','); 
} 

static void print_date(TinyGPS &gps) {
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age != TinyGPS::GPS_INVALID_AGE) { 
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d,",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  Serial.print(',');
}

static void gy_bno055() {
  sensors_event_t bno;
  i2c.getEvent(&bno, Adafruit_BNO055::VECTOR_EULER); 
  Serial.print(i2c.getTemp());
  Serial.print(',');
  Serial.print(bno.orientation.x);
  Serial.print(',');
  Serial.print(bno.orientation.y);
  Serial.print(',');
  Serial.print(bno.orientation.z); 
  Serial.print(',');
} 

static void voltmeter() {
  int sensorValue = analogRead(A6); 
  float voltage = sensorValue / 1023.0 * 5.0; 
  Serial.print(voltage);
}
