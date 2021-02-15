 ## Rover ver 1.3 @ Jul 00 2020
#### This is a rover that could be controlled over the internet.<br>
![alt text](https://raw.githubusercontent.com/xg590/rover/v1.1/github/architecture.png "architecture")
* User can use a Xbox Wireless Controller to steer the rover, tweak the angle of on-board camera. 
* A website is set up to track the GPS position of rover and display the real-time image captured from the rover. 
#### Architecture Description
1. The Controller is linked to a Raspberry Pi (RPi) Zero W or PC via Bluetooth.
2. Commands came from the Controller are sent to a publicly accessible Web Server.
3. The Server forwards processed commands to the Rover, who would move around accordingly. 
4. Real-time image and sensor reading are sent back to the web server, where the image is stored on the fly while sensor data is processed and logged in a database.
6. A web browser of user can access the webpage on server, fetch the latest image and GPS trace, and render them for user's inspection. 
* Besides, all communication are protected by SSL
#### Photo
![alt text](https://github.com/xg590/rover/blob/small/github/photo.jpg "real rover")
#### Hardware on board:
  1. Raspberry Pi: Zero W
  2. Arudino: Pro Mini 5V
  3. Servo: SG90
  4. GPS: GY-NEO6MV2
  5. Compass: GY-BNO055
  6. Logic Level Converter: 4 Channels Bi-Directional 3.3V-5V Converter
  6. Camera: Kuman Infrared Camera
  7. Motor Driver: PCA9685 + TB6612 DC/Stepper Motor Driver Board
  8. Battery Managment System: [2A 5V Charge Discharge Integrated Module](https://www.amazon.com/dp/B07PZT3ZW2)
  9. N-Channel MOSFET: RFP30N06LE (30A 60V Enhancement_Mode Logic_Level_Gate)
#### Usage
##### Server
* Register a domain (Google Domain)
* Setup an AWS account and get AWS IAM [access credentials](https://docs.aws.amazon.com/general/latest/gr/aws-sec-cred-types.html). 
* Run setup/Use_LightSail_as_Server.ipynb with Access key ID (for example, AKIAIOSFODNN7EXAMPLE) and secret access key at hand.
* Log in the server and run setup/secret.sh
##### Rover
```shell
ssh -i lightsail -NfR 2222:127.0.0.1:3333 -l ubuntu guoxiaokang.net && raspivid --rotation 90 --timeout 0 --width 640 --height 360 --bitrate 1000000 --framerate 25 --profile baseline --listen -o tcp://127.0.0.1:3333 &
```
##### Driver
* Connect Xbox Wireless Controller to a win10 laptop.
* 

#####
* Enable I2C interface of on-board raspberry pi to communicate with Motor Driver Board
* Install python libraries for on-board raspberry pi
```shell
sudo apt install python3-pip python3-pigpio  # servo
pip3 install adafruit-circuitpython-motorkit # motor
```
* Since additional libraries are needed in Arduino IDE, let's install them on Raspberry Pi
```
sudo ln -s /home/pi/arduino-1.8.13/arduino /usr/local/bin/arduino
arduino --install-library TinyGPS
arduino --install-library "Adafruit Unified Sensor"
arduino --install-library "Adafruit BNO055"
arduino --board arduino:avr:nano:cpu=atmega328old --port /dev/ttyUSB0 --upload Sensor.ino
```
* Disconnect VCC pin of GPS breakout before program the arduino board (Uno for testing)
```
arduino --board arduino:avr:nano:cpu=atmega328old --port /dev/ttyUSB0 --upload Sensor.ino
``` 
#### Raspberry Pi Streaming Problem
What I want
* Low latency
* Low burden for the raspberry pi on rover
* Secure video transmission between rover and intermediate server. 
* Least library dependency on rover (Install package is a nightmare on raspberry pi)

Solution
1. Avoid python and use <i>raspivid</i> 
2. Redirect stdout (video stream in h264) of raspivid to server via <i>ssh</i>
3. Encoding on the intermediate server and then distribute in webm (maybe)

Implement (Proof of concept)
1. Capture video and listen on a local port 3333 on raspberry pi (it turned out that there is no noticeable burden on raspberry pi)
```shell
raspivid --timeout 0 --width 640 --height 360 --bitrate 1000000 --framerate 25 --profile baseline --listen -o tcp://127.0.0.1:3333
```
2. Initiate a remote port forwarding from raspberry pi and listen on port 2222 of remote server 
```shell
ssh -R 2222:127.0.0.1:3333 remote_server
```
3. View the stream from remote_server
````shell
ffplay -i tcp://127.0.0.1:2222
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
