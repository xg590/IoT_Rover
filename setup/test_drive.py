# 5C:BA:37:CB:B9:35
# sudo bash -c "echo -e 'connect XX:XX:XX:XX:XX:XX\nexit' | bluetoothctl"
# sudo pigpiod -p 8889

# from evdev import InputDevice, ecodes
# device = InputDevice('/dev/input/event0') 
# for event in device.read_loop():
#   if event.type in [ecodes.EV_ABS, ecodes.EV_KEY]:
#       # value = (32768-event.value)/32768
#       if event.code in [0, 1]: 
#           pass
#       else:   
#           print(event.code, event.value)
import subprocess, socket
def startGPIOdaemon():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(2)                                            #2 Second Timeout
    result = sock.connect_ex(('127.0.0.1', 8889))
    if result == 0:
        print( 'pigpiod is running' )
    else:
        subprocess.run("sudo pkill pigpiod", shell=True)
        subprocess.run("sudo pigpiod -p 8889", shell=True) 
        print( 'pigpiod was just started' ) 
    sock.close()
startGPIOdaemon()

from evdev import InputDevice, ecodes
device = InputDevice('/dev/input/event0')

code2name = {}
for type_, code_ in device.capabilities(verbose=True, absinfo=False).items():
    if type_[0] in ['EV_KEY', 'EV_ABS']:
        for name, code in code_:
            if isinstance(name,list):
                if   'BTN_A' in name: code2name[code] = 'BTN_A'
                elif 'BTN_B' in name: code2name[code] = 'BTN_B'
                elif 'BTN_X' in name: code2name[code] = 'BTN_X'
                elif 'BTN_Y' in name: code2name[code] = 'BTN_Y'
                continue
            code2name[code] = name

#     'ABS_X', # default value for left  stick X
#     'ABS_Y', # default value for left  stick Y
#     'ABS_Z', # default value for right stick X
#    'ABS_RZ', # default value for right stick Y
#   'ABS_GAS', # default value for right trigger
# 'ABS_BRAKE', # default value for left  trigger

import pigpio
pi = pigpio.pi(port=8889) ### Start PiGPIO Client

import time
from adafruit_motorkit import MotorKit
kit = MotorKit()

class XBoxController():
    def __init__(self):
        self.rota = 2300
        pi.set_servo_pulsewidth(20, 2300) # rota 600~1050 back~2300 front~2400
        self.elev = 1850
        pi.set_servo_pulsewidth(21, 1850) # elev up 1500~1850~2500 down
        self.sensitivity = 10
        self.threshold = 0.5

    def servo_elev(self, x): 
        x = (32768-x)/32768
        if x < self.threshold and x > -1*self.threshold: return    
        self.elev -= x*self.sensitivity
        if self.elev > 2500:
            self.elev = 2500
        elif self.elev < 1500:
            self.elev = 1500
        else:
            pi.set_servo_pulsewidth(21, self.elev)

    def servo_rota(self, x):
        x = (32768-x)/32768
        if x < self.threshold and x > -1*self.threshold: return    
        self.rota += x*self.sensitivity 
        if self.rota > 2400:
            self.rota = 2400
        elif self.rota < 600:
            self.rota = 600
        else:
            pi.set_servo_pulsewidth(20, self.rota)

    def motor_left(self, x):
        if x < 0.5 and x > -0.5: x = 0
        kit.motor1.throttle = x
        kit.motor2.throttle = x

    def motor_right(self, x):
        if x < 0.5 and x > -0.5: x = 0
        kit.motor3.throttle = x
        kit.motor4.throttle = x
xbox = XBoxController()

for event in device.read_loop():
    if event.type in [ecodes.EV_ABS, ecodes.EV_KEY]:
        # value = (32768-event.value)/32768
        if event.code == 1: # Y up 0 
            xbox.servo_elev(event.value)
        elif event.code == 0: 
            xbox.servo_rota(event.value)
        elif event.code == 158: # front
            if event.value == 1:
                pi.set_servo_pulsewidth(20, 2300)
                pi.set_servo_pulsewidth(21, 1850)
            else:
                pi.set_servo_pulsewidth(20, xbox.rota)
                pi.set_servo_pulsewidth(21, xbox.elev) 
        elif event.code == 315: # front
            if event.value == 1:
                pi.set_servo_pulsewidth(20, 1050)
                pi.set_servo_pulsewidth(21, 1850)
            else:
                pi.set_servo_pulsewidth(20, xbox.rota)
                pi.set_servo_pulsewidth(21, xbox.elev)   
    # {158: 'KEY_BACK', 172: 'KEY_HOMEPAGE', 304: 'BTN_A', 305: 'BTN_B', 306: 'BTN_C', 307: 'BTN_X', 308: 'BTN_Y', 309: 'BTN_Z', 310: 'BTN_TL', 311: 'BTN_TR', 312: 'BTN_TL2', 313: 'BTN_TR2', 314: 'BTN_SELECT', 315: 'BTN_START', 316: 'BTN_MODE', 317: 'BTN_THUMBL', 318: 'BTN_THUMBR', 0: 'ABS_X', 1: 'ABS_Y', 2: 'ABS_Z', 5: 'ABS_RZ', 9: 'ABS_GAS', 10: 'ABS_BRAKE', 16: 'ABS_HAT0X', 17: 'ABS_HAT0Y'}
        elif event.code ==  9:
            xbox.motor_left(event.value/ 1023); xbox.motor_right(event.value/ 1023)
        elif event.code == 10:
            xbox.motor_left(event.value/-1023); xbox.motor_right(event.value/-1023)
        elif event.code == 310:
            xbox.motor_left(event.value*-1   ); xbox.motor_right(event.value      )
        elif event.code == 311:
            xbox.motor_left(event.value      ); xbox.motor_right(event.value*-1   )