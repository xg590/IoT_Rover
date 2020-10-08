## Rover ver 1.3 @ Jul 00 2020
#### This is a rover that could be controlled over the internet.<br>
![alt text](https://raw.githubusercontent.com/xg590/rover/v1.1/github/architecture.png "architecture")
* User can use a Xbox Wireless Controller to steer the rover, tweak the angle of on-board camera and manipulate a robot arm. 
* A website is set up to track the GPS position of rover and display the real-time image captured from the rover. 
#### Architecture Description
1. The Controller is linked to a Raspberry Pi (RPi) Zero W or PC via Bluetooth.
2. Commands came from the Controller are sent to a publicly accessible Web Server.
3. The Server forwards processed commands to the Rover, who would move around accordingly. 
4. Real-time image and sensor reading are sent back to the web server, where the image is stored on the fly while log is processed and stored in a database.
6. A web browser of user can access the webpage on server, fetch the latest image and GPS log, and render them for user's inspection. 

* Besides, all communication are protected by SSL
#### Photo
![alt text](https://github.com/xg590/rover/blob/v1.2/github/photo.jpg "real rover")
#### Hardware on board:
  1. Raspberry Pi 3A+
  2. Arudino Pro Mini
  3. Servo: MG995 / MG996R
  4. GPS: GY-NEO6MV2
  5. Compass: GY-BNO055
  6. IR Cam: Kuman RPi Camera
  7. Motor Driver: PCA9685 + TB6612 DC/Stepper Motor Driver Board
  8. Servo Driver: PCA9685 16-Channel Servo Driver
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
#### Streaming Problem
What I want
1. Low latency
2. Low burden for the raspberry pi on rover
3. Secure video transmission between rover and intermediate server. 
4. Least library dependency

Solution
* Avoid python and use <i>raspivid</i>
* Redirect stdout (raw in h264) of raspivid to server via <i>ssh</i>
* Encoding on the intermediate server and then distribute in webm (maybe)

Implement (Proof of concept)
* Capture video and sent it to a local port 3333 on raspberry pi
```shell
raspivid -t 0 -fps 25 -w 640 -h 480 -o - | nc 127.0.0.1 3333
```
* Forward the local port 3333 to a remote port 2222 (I tested it on a third machine)
```shell
ssh -R 3333:intermediate_server:2222 pi@rover
```
* Listen on 2222 and feed the raw data to mplayer
````shell
nc -l 2222 | mplayer -fps 25 -demuxer h264es -
````
#### New in v1.3 
1. Communication between Arduino and Raspberry Pi is now in UART to save a USB port
2. A mechanical arm is installed.
3. A second non-Infrared is added and it can move 360.

#### New in v1.2
1. Offload the communication with sensors to Arduino Uno
2. TCP Communication can be interrupted by network error and will resume automatically after that.
 
#### New in v1.1
1. All resources are localized if possible.
2. Use tcp for the sensor data communication as shown above. The first version actually use http which is expensive for on-board RPi. 
