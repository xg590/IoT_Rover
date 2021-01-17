/*
sudo ln -s /home/pi/arduino-1.8.13/arduino /usr/local/bin/arduino
arduino --install-library TinyGPS
arduino --install-library "Adafruit Unified Sensor"
arduino --install-library "Adafruit BNO055"
arduino --board arduino:avr:nano:cpu=atmega328old --port /dev/ttyUSB0 --upload Sensor.ino
import serial
with serial.Serial(port='/dev/ttyUSB0', baudrate=9600, timeout=3) as s:
    while 1: print(s.readline())
*/
#include <SoftwareSerial.h>
#include <Wire.h>
#include <TinyGPS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>  
SoftwareSerial gps_uart(A1, A2); // (RX, TX). Digital Pin A2 acts as Rx pin which should be wired to Tx pin of GPS breakout board.
TinyGPS tinygps;
Adafruit_BNO055 compass_i2c = Adafruit_BNO055(55, 0x29); // wire A4/A5 to SDA/SCL.
void setup()
{
  compass_i2c.begin();       // Compass
  delay(1000);               // Delay for compass readiness
  Serial.begin(9600);
  gps_uart.begin(9600);
}
void loop()
{
    unsigned long interval = 900; // give TinyGPS 900 millseconds to collect GPS output then we can read position.
    bool newFix = false; // true if we got GPS fix/position
    for (unsigned long start = millis(); millis() - start < interval;)
    {
        while (gps_uart.available())
        {
            char c = gps_uart.read();
            // Serial.write(c);
            if (tinygps.encode(c)) // Did a new valid sentence come in?
                newFix = true;
        }
    }
    Serial.print("{") ;
    gy_bno055();
    voltmeter(A3, "v");
    if (newFix) gy_neo6mv2();
    else {
      Serial.print(",\"gps\":0");
      digitalWrite(LED_BUILTIN, HIGH); 
      delay(100);                      
      digitalWrite(LED_BUILTIN, LOW);  
    }    
    Serial.println("}");
}
static void gy_neo6mv2() {
    float flat, flon;
    unsigned long fix_age;/* http://arduiniana.org/libraries/tinygps/
    To test whether the TinyGPS object contains valid fix data,
    pass the address of an unsigned long variable for the “fix_age”
    parameter in the methods that support it. If the returned value
    is TinyGPS::GPS_INVALID_AGE, then you know the object has never
    received a valid fix. If not, then fix_age is the number of
    milliseconds since the last valid fix. If you are “feeding” the
    object regularly, fix_age should probably never get much over 1000.
    If fix_age starts getting large, that may be a sign that you once
    had a fix, but have lost it.*/
    tinygps.f_get_position(&flat, &flon, &fix_age);
    if (fix_age == TinyGPS::GPS_INVALID_AGE)
        Serial.print(",\"gps\":0");
    else {
        Serial.print(",\"lat\":") ; Serial.print(flat, 6               );
        Serial.print(",\"lon\":") ; Serial.print(flon, 6               );
        Serial.print(",\"alt\":") ; Serial.print(tinygps.f_altitude()  );
        Serial.print(",\"cus\":") ; Serial.print(tinygps.f_course()    );
        Serial.print(",\"spd\":") ; Serial.print(tinygps.f_speed_kmph());
        Serial.print(",\"sat\":") ; Serial.print(tinygps.satellites()  );
        int year;
        byte month, day, hour, minute, second, hundredths;
        tinygps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &fix_age);
        char sz[32];
        sprintf(sz, ",\"ts\":\"%02d-%02d-%02dT%02d:%02d:%02d\",\"gps\":1", year, month, day, hour, minute, second);
        Serial.print(sz);
    }
}
static void gy_bno055() {
    sensors_event_t bno;
    compass_i2c.getEvent(&bno, Adafruit_BNO055::VECTOR_EULER);
    Serial.print("\"T\":") ; Serial.print(compass_i2c.getTemp());
    Serial.print(",\"x\":") ; Serial.print(bno.orientation.x);
    Serial.print(",\"y\":") ; Serial.print(bno.orientation.y);
    Serial.print(",\"z\":") ; Serial.print(bno.orientation.z);
}
static void voltmeter(int pinId, char const volId[1]) {
    int sensorValue = analogRead(pinId);
    float voltage = sensorValue / 1023.0 * 5.0;
    Serial.print(",\"") ; Serial.print(volId) ; Serial.print("\":") ; Serial.print(voltage, 2);
}
