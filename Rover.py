yaw_pin      = 4  
pitch_pin    = 17
gpio_port    = 8889 
control_port = 54322  
sensor_port  = 54324
server_ip    = 'guoxiaokang.net' 
retry_in     = 5 # seconds

import subprocess, socket, pigpio
def startGPIOdaemon():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(2)                                            #2 Second Timeout
    result = sock.connect_ex(('127.0.0.1', gpio_port))
    if result == 0:
        print( 'pigpiod is running' )
    else:
        subprocess.run("sudo pkill pigpiod", shell=True)
        subprocess.run("sudo pigpiod -p 8889", shell=True) 
        print( 'pigpiod was just started' ) 
    sock.close()
startGPIOdaemon()

import pigpio, time, adafruit_motorkit
pi  = pigpio.pi(port=gpio_port) ### Start PiGPIO Client http://abyz.me.uk/rpi/pigpio/pigpiod.html
kit = adafruit_motorkit.MotorKit()  
 
# Establish connection with server
import ssl, struct, threading   
server_cert = 'setup/secret/server.crt'
client_cert = 'setup/secret/rover.crt'
client_key  = 'setup/secret/rover.key'  

context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=server_cert)
context.load_cert_chain(certfile=client_cert, keyfile=client_key) 

def control_link_daemon():
    l = struct.calcsize('<Bff')
    while True: 
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        ss = context.wrap_socket(s, server_side=False, server_hostname=server_ip) 
        ss.settimeout(retry_in)  
        try: 
            print('[Control Link] Try to connect the server') 
            ss.connect((server_ip, control_port))  
            print('[Control Link] Succeed a connection')  
        except (socket.timeout, ConnectionRefusedError, ConnectionResetError):
            print(f'[Control Link] Failed to connect the server and retry the connection in {retry_in} seconds')
            time.sleep(retry_in) 
            continue 
        while True:
            try:
                command_type, value_1, value_2 = struct.unpack('<Bff', ss.recv(l)) 
                # print(command_type, value_1, value_2)
            except Exception as e:
                ss.shutdown(socket.SHUT_RDWR) 
                ss.close()
                print(f'[Control Link] Connection closed and will reopen in {retry_in} seconds', e)
                time.sleep(retry_in)
                break 
            if   command_type == 1:
                pi.set_servo_pulsewidth(pitch_pin, value_1)
                pi.set_servo_pulsewidth(yaw_pin, value_2) 
            elif command_type == 2:  
                kit.motor1.throttle = value_1
                kit.motor2.throttle = value_1 
                kit.motor3.throttle = value_2
                kit.motor4.throttle = value_2  
                
control_link_thread = threading.Thread(target=control_link_daemon); control_link_thread.start(); print('control_link_daemon', control_link_thread.getName())

import serial 
def sensor_link_daemon():
    sensor = serial.Serial(port='/dev/ttyUSB0', baudrate=9600, timeout=3) # serial commu with GPS chip  
    while True:   
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        ss = context.wrap_socket(s, server_side=False, server_hostname=server_ip) 
        ss.settimeout(retry_in)  
        try: 
            print('[Sensor  Link] Trying to connect the server')
            ss.connect((server_ip, sensor_port))
            print('[Sensor  Link] Succeed a connection')  
        except (ConnectionRefusedError, ConnectionResetError, socket.timeout):
            print(f'[Sensor  Link] Failed to connect the server and retry the connection in {retry_in} seconds')
            time.sleep(retry_in)
            continue
        try:
            while True: 
                data = sensor.readline() 
                ss.send(struct.pack('<H', len(data))) 
                ss.send(data)
        except socket.timeout as e: 
            ss.shutdown(socket.SHUT_RDWR)
            ss.close()
            print(f'[Sensor  Link] Time out and closed. Reestablish in {retry_in} seconds:', e)
            time.sleep(retry_in) 
        except (ConnectionResetError, BrokenPipeError) as e:
            print(f'[Sensor  Link] Server ended the connection. Reestablish in {retry_in} seconds:', e)
            time.sleep(retry_in) 

sensor_link_thread = threading.Thread(target=sensor_link_daemon); sensor_link_thread.start(); print('sensor_link_daemon', sensor_link_thread.getName())
