### Test SG90 Servo 
Library
```
sudo apt install python3-pigpio 
```
Start the daemon
```
sudo pigpiod -p 8889
```
Python
```python
import pigpio
pi = pigpio.pi(port=8889) ### Start PiGPIO Client 
pi.set_servo_pulsewidth(21, 0) # ele
```
### Test PCA9685_TB6612
* Install library
```
pip3 install adafruit-circuitpython-motorkit 
```
* Run code
```python  
import time
from adafruit_motorkit import MotorKit
kit = MotorKit() 
def motor(f): 
	kit.motor1.throttle = f
	kit.motor2.throttle = f
	kit.motor3.throttle = f
	kit.motor4.throttle = f 
  
motor(1)
time.sleep(1) 
motor(0)

```
### Sensor
* Program arduino on Linux
./arduino-1.8.13/arduino --board arduino:avr:nano:cpu=atmega328 --port /dev/ttyUSB0 --upload Sensor.ino
* Test Sensor
``` 
import serial
with serial.Serial(port='/dev/ttyS0', baudrate=9600, timeout=2) as s:
    while 1: print(s.readline()) 
```
