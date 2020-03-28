/*
sudo ln -s /home/pi/arduino-1.8.12/arduino /usr/local/bin/arduino
arduino --install-library "Adafruit Unified Sensor"
arduino --install-library "Adafruit BNO055" 
arduino --board arduino:avr:uno --port /dev/ttyUSB0 --upload sensor.ino

import serial
with serial.Serial(port='/dev/ttyUSB0', baudrate=115200, timeout=2) as s:
    while 1: print(s.readline())
*/
 
#include <SoftwareSerial.h> 
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Wire.h>
#define BNO055_SAMPLERATE_DELAY_MS (100) 
Adafruit_BNO055 i2c = Adafruit_BNO055(55, 0x29); 

void setup(void) {
  Serial.begin(115200); 
  i2c.begin();      // compass
  delay(1000);      // delay for compass
}

void loop() {  
  Serial.print("$BNO055,");
  gy_bno055(); 
  Serial.print(',');
  voltmeter(A0); // 4V
  Serial.print(',');
  voltmeter(A2); // 12V 
  Serial.println();
  delay(BNO055_SAMPLERATE_DELAY_MS);
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
} 

static void voltmeter(int pinId) {
  int sensorValue = analogRead(pinId);
  float voltage = sensorValue / 1023.0 * 5.0; 
  Serial.print(voltage);
}
 