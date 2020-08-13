## Rover ver 1.3 @ Jul 00 2020
#### This is a rover that could be controlled over the internet.<br>
![alt text](https://raw.githubusercontent.com/xg590/rover/v1.1/github/architecture.png "architecture")
User can use a Xbox Wireless Controller to steer the rover, tweak the angle of on-board camera and manipulate a robot arm. A website is set up to track the GPS position of rover and display the real-time image captured from the rover. 
#### Architecture Description
1. The Controller is linked to a Raspberry Pi (RPi) Zero W or PC via Bluetooth.
2. RPi sends commands from Controller to a publicly accessible Web Server.
3. The Server forwards processed commands to the Rover.
4. Rover moves around accordingly as the on-board camera and sensors (gps, compass, thermometer) record.
5. Image and sensor log are sent back to web server, where the image is stored on the fly while log is processed and stored in a database.
6. A web browser of user can access the webpage on server, fetch the latest image and GPS log, and render them for inspection. 

* Besides, all communication are protected by SSL (TCP/HTTP over TLS)
#### Photo
![alt text](https://github.com/xg590/rover/blob/v1.2/github/photo.jpg "real rover")
#### Hardware on board:
  1. Raspberry Pi 3B
  2. Arudino Uno
  3. Servo: SG90
  4. GPS: GY-NEO6MV2
  5. Compass: GY-BNO055
  6. IR Cam: Kuman RPi Camera
  7. Motor Driver: PCA9685 + TB6612 DC/Stepper Motor Driver Board 
#### Usage
* Since additional libraries are needed in Arduino IDE, let's install them on Raspberry Pi
```
pi@raspberrypi:~ $ ./arduino --install-library "TinyGPS" 
pi@raspberrypi:~ $ ./arduino --install-library "Adafruit BNO055" 
pi@raspberrypi:~ $ ./arduino --install-library "Adafruit Unified Sensor"
```
* Disconnect VCC pin of GPS breakout before program the arduino board (Uno for testing)
```
pi@raspberrypi:~ $ ./arduino --board arduino:avr:uno --port /dev/ttyUSB0 --upload sensor.ino 
```
#### New in v1.2
1. Offload the communication with sensors to Arduino
2. TCP Communication can be interrupted by network error and will resume automatically after that.
 
#### New in v1.1
1. All resources are localized if possible.
2. Use tcp for the sensor data communication as shown above. The first version actually use http which is expensive for on-board RPi.

