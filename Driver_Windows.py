server_ip, control_port = ('guoxiaokang.net', 54322)

### Gather commands from controller 
class Camera_Control():
    def __init__(self):
        self.pitch_level = 1870 # 1500(up)~1870(hori)~2500(down) 
        self.yaw_front = 1870   # 650(back)~1870(front) 
        self.yaw_end = 650
        self.pitch = self.pitch_level 
        self.yaw = self.yaw_front
        self.pitch_sensitivity = 100
        self.yaw_sensitivity = 100

    def change_pitch(self, x):
        self.pitch += x*self.pitch_sensitivity
        if   self.pitch > 2500: self.pitch = 2500
        elif self.pitch < 1500: self.pitch = 1500 

    def change_yaw(self, x):   
        self.yaw -= x*self.yaw_sensitivity 
        if   self.yaw > 2500: self.yaw = 2500
        elif self.yaw <  600: self.yaw =  600  
cam_ctrl = Camera_Control()

import time, sys, queue, threading, ssl, socket, struct 
q = queue.Queue() # cache the command to be send

def heartbeat_daemon():
    '''
    Send an invalid action to keep the connection alive
    would be ignored by rover but prevent a timeout for the control link
    '''
    while True: 
        q.put((1, 0, 0)) 
        time.sleep(3)
threading.Thread(target=heartbeat_daemon).start() 

def controller_daemon(): 
    import pygame # https://www.pygame.org/docs/ref/joystick.html
    
    pygame.init()
    clock = pygame.time.Clock()
    if pygame.joystick.get_count():
        joystick = pygame.joystick.Joystick(0)
        print(f"Joystick name: {joystick.get_name()}") 
    else:
        print('No joystick')
        sys.exit(0)
    '''
    Key map reference
    buttonMap = {0:'A', 1:'B', 2:'X', 3:'Y', 
                 4:'LB', 5:'RB', # Left/Right shoulder button 
                 6:'V', # View 
                 7:'M', # Menu
                 8:'LS', 9:'RS' # Left/Right stick in
                }
    axisMap = {0:'X', 1:'Y', 2:'RX', 3:'RY', 4:'LT', 5:'RT'} 
    '''
    axis_history = {0:-1, 1:-1, 2:-1, 3:-1, 4:-1, 5:-1}  
    axis_throttle_threshold = 0.5 # setup a threshold to filter unintentional axis motion
    axis_motion_threshold   = 0.3 # setup a threshold to filter unintentional axis motion
    axis_motion_sensitivity = 0.1 # motion change under this threshold will be ignored.
    while True: # Credits to CodeSurgeon @ https://stackoverflow.com/a/46558178 
        '''
        Logic: loop while listen joystick events. 
               At the beginning of each new loop, 
               get event happended during the last loop.
        ''' 
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.JOYAXISMOTION:
                axis, value = event.axis, event.value 
                if   axis == 4: 
                    value = (value + 1) /  2 # LT/Break
                    if value < axis_throttle_threshold: value = 0
                elif axis == 5: 
                    value = (value + 1) / -2 # RT/Gas  
                    if abs(value) < axis_throttle_threshold: value = 0
                elif axis in [0, 1, 2, 3]:
                    if abs(value) < axis_motion_threshold: value = 0 
                    
                if abs(axis_history[event.axis] - value) < axis_motion_sensitivity: continue
                else: axis_history.update({axis:value}) 
                
                if   axis == 0:
                    cam_ctrl.change_yaw(value)
                    q.put((1, cam_ctrl.pitch, cam_ctrl.yaw))
                elif axis == 1:
                    cam_ctrl.change_pitch(value)
                    q.put((1, cam_ctrl.pitch, cam_ctrl.yaw))
                elif axis in [4, 5]: 
                    q.put((2, value, value))  
            elif event.type == pygame.JOYBUTTONDOWN:
                if   event.button == 4:
                    q.put((2, 1, -1))
                elif event.button == 5:
                    q.put((2, -1, 1))
                elif event.button == 6: # look back
                    q.put((1, cam_ctrl.pitch_level, cam_ctrl.yaw_end))
                elif event.button == 7: # reset cam view 
                    cam_ctrl.pitch, cam_ctrl.yaw = cam_ctrl.pitch_level, cam_ctrl.yaw_front
                    q.put((1, cam_ctrl.pitch, cam_ctrl.yaw))
            elif event.type == pygame.JOYBUTTONUP:
                if   event.button == 4:
                    q.put((2, 0, 0))
                elif event.button == 5:
                    q.put((2, 0, 0))
                elif event.button == 6:   
                    q.put((1, cam_ctrl.pitch, cam_ctrl.yaw))
            elif event.type == pygame.JOYHATMOTION:
                pass# print(event.hat, event.value) 
            elif event.type == pygame.JOYDEVICEREMOVED: # pygame.JOYDEVICEADDED:
                print("Joystick removed.")  
                break
        else:
            clock.tick(10) # ensure 10 loops per second
            continue
        print('Stop getting any new event because no joystick~')
        break 
threading.Thread(target=controller_daemon).start() 

### Send commands to Server

# Establish connection with server
server_cert = 'setup/secret/server.crt'
client_cert = 'setup/secret/driver.crt'
client_key  = 'setup/secret/driver.key' 
retry_in    = 5

context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH, cafile=server_cert)
context.load_cert_chain(certfile=client_cert, keyfile=client_key) 

def control_link_daemon():
    while True: 
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        ss = context.wrap_socket(s, server_side=False, server_hostname=server_ip) 
        ss.settimeout(retry_in)  
        try: 
            print('Try to connect the server') 
            ss.connect((server_ip, control_port))  
            print('Succeed a connection')  
        except (socket.timeout, ConnectionRefusedError, ConnectionResetError):
            print(f'Failed to connect the server and retry the connection in {retry_in} seconds')
            time.sleep(retry_in) 
            continue
        while True:
            try:
                item = struct.pack('<Bff', *q.get()) # https://docs.python.org/3/library/struct.html#format-characters
                # print(struct.unpack('<Bff', item))
                ss.send(item) # Send code and value to server  
            except socket.timeout as e: 
                ss.shutdown(socket.SHUT_RDWR)
                ss.close()
                break
            except (ConnectionAbortedError, ConnectionResetError, BrokenPipeError) as e:
                print(f'Server ended the connection and connection reestablishment will be retried in {retry_in} seconds:', e)
                time.sleep(retry_in) 
                break
threading.Thread(target=control_link_daemon).start()

