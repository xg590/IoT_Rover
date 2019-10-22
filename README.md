## Rover ver 1.1 @ Oct 20 2019
#### This is a rover that could be controlled over the internet.<br>
User can use a Xbox Wireless Controller to drive the rover and tweak the angle of on-board camera. A website is set up to track the GPS position of rover and display the real-time image captured from the rover. In addition, a gradienter on the lower right corner of the website gives user the information about terrant. 
#### Architecture Description
![alt text](https://raw.githubusercontent.com/xg590/rover/v1.1/github/architecture.png "architecture")
1. The Controller is linked to a Raspberry Pi (RPi) Zero W via Bluetooth.
2. RPi sends commands from Controller to a publicly accessible Web Server.
3. The Server forwards processed commands to the Rover.
4. Rover moves around accordingly as the on-board camera and sensors (gps, compass, accelerometer, thermometer) record.
5. Image and sensor log are sent back to web server, where the image is stored on the fly while log is processed and stored in a database.
6. A web browser of user can access the webpage on server, fetch the latest image and GPS log, and render them for inspection. 

* Besides, all communication are protected by SSL (TCP/HTTP over TLS)
#### Photo
![alt text](https://raw.githubusercontent.com/xg590/rover/v1.1/github/photo.jpg "real rover")
#### Hardware on board:
  1. GPS: GY-NEO6MV2
  2. Compass, accelerometer: Micro:Bit 
  3. Motor Driver: DRV8833 DC/Stepper Breakout Board
  4. IR Cam: Kuman RPi Camera
  5. 2-DOF Gimbal and Steering System: 9g Servo
  6. Raspberry Pi Zero W
  
#### New in v1.1
1. All resources are localized if possible.
2. Use tcp for the sensor data communication as shown above. The first version actually use http which is expensive for on-board RPi.

#### Plan
1. Scripts to automate setup. Now, I have to login each participants and run the python script manually.
2. Imporve communication reliability. Reconnect after network interruption. 
