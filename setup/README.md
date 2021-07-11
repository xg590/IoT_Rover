#### Libraries
```
sudo apt install screen python3-pigpio python3-pip
pip3 install adafruit-circuitpython-motorkit pyserial
```
### Test SG90 Servo  
Start the daemon
```
sudo pigpiod -p 8889
```
Python
```python
YAW_PIN = 26
PITCH_PIN = 19

import pigpio
pi = pigpio.pi(port=8889) ### Start PiGPIO Client 
pi.set_servo_pulsewidth(YAW_PIN, 1000)
pi.set_servo_pulsewidth(PITCH_PIN, 1000)
```
### Test PCA9685_TB6612 
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
  
motor(1); time.sleep(1); motor(0)
```
### Program and test sensors
* Install Arduino IDE on Raspberry Pi. ([Tutorial](https://github.com/xg590/IoT/blob/master/Arduino/README.md#install-arduino-ide-on-raspberry-pi))
* Install Libraries
```
arduino --install-library "TinyGPS"
arduino --install-library "Adafruit BNO055"
arduino --install-library "Adafruit Unified Sensor"
```
* Program
```
arduino --board arduino:avr:nano:cpu=atmega328 --port /dev/ttyUSB0 --upload Sensor.ino
```
* Test
``` 
import serial
with serial.Serial(port='/dev/ttyS0', baudrate=9600, timeout=3) as s:
    while 1: print(s.readline()) 
``` 
### Connect Bluetooth
1) Disable ERTM (Enhanced Re-Transmission Mode)
```
sudo bash -c "echo 1 > /sys/module/bluetooth/parameters/disable_ertm"
```
2) Put controller into Discovery Mode <br>
3) Pair Controller with Raspberry Pi via terminal bluetooth utility [1]
```
$ sudo su
# bluetoothctl 
[bluetooth]# scan on 
[bluetooth]# pair 00:9E:C8:63:EA:71
[bluetooth]# connect 00:9E:C8:63:EA:71
[bluetooth]# exit 
```
### Jupyter
```
mkdir ~/.jupyter
cat << EOF > ~/.jupyter/jupyter_notebook_config.py 
c.NotebookApp.ip = '*'
c.NotebookApp.port = 8888 
c.NotebookApp.open_browser = False
c.NotebookApp.password = u'sha1:ffed18eb1683:ee67a85ceb6baa34b3283f8f8735af6e2e2f9b55' 
EOF
sudo apt update && sudo apt install python3-pip python3-venv
python3 -m venv rover
source rover/bin/activate
pip install jupyter jupyter_contrib_nbextensions
jupyter contrib nbextension install --user
```
