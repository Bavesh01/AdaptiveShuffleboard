#!/usr/bin/env python
import io
import tkinter as tk
from PIL import Image, ImageTk
import serial
import keyboard
import time

class GUI:
    def __init__(self, master):
        self.master = master
        master.title("Shuffleboard Homescreen")
        master.attributes('-fullscreen', True)
        self.state = '000'

        self.states = {
            '000' : 'MENU',
            '010' : 'AIM_SELECT',
            '020' : 'SPIN_SELCT',
            '030' : 'ANGLE_SELECT',
            '040' : 'POWER_SELECT',
            '100' : 'AIM_MODE_ZER0',
            '101' : 'AIM_MODE_RIGHT',
            '102' : 'AIM_MODE_LEFT',
            '200' : 'SPIN_MODE_ZER0',
            '300' : 'ANGLE_MODE_ZERO',
            '400' : 'POWER_MODE_ZERO',
            '999' : 'SHOOTING',
            '002' : 'MOVE_MODE',
            '001' : 'HOMING',
            '777' : 'PRECISION_PRECISION',
            '778' : 'PRECISION_ARCADE',
            '666' : 'ARCADE_PRECISION',
            '667' : 'ARCADE_ARCADE',
            '720' : 'ARCADE_SPIN',
            '721' : 'ARCADE_SPIN_R',
            '719' : 'ARCADE_SPIN_L',
            '730' : 'ARCADE_LAUNCH',
            '888' : 'ARCADE_LAUNCH',
         }
        for i in range(1,6):
            self.states[str(200+i)] = 'SPIN_MODE_RIGHT' + str(i)
            self.states[str(200-i)] = 'SPIN_MODE_LEFT' + str(i)
            self.states[str(730+i)] = 'SPIN_MODE_RIGHT' + str(i)
            self.states[str(730-i)] = 'SPIN_MODE_LEFT' + str(i)
        for i in range(1,17):
            self.states[str(400+i)] = 'POWER_MODE_' + str(i)
        #print(self.states)
        

        self.gallery = {s:ImageTk.PhotoImage(Image.open(f'images/{s}.PNG').resize((800,450)))
                   for s in self.states.keys()}
        
        
        time.sleep(0.5)
        self.canvas = tk.Canvas(self.master, width=800, height=490)
        self.canvas.pack(expand=True, fill=tk.BOTH)

        try:
            self.ser = serial.Serial('/dev/ttyACM0', 2000000)
        except:
            self.ser = serial.Serial('/dev/ttyACM1', 2000000)


        self.master.after(1, self.check_serial)
        self.update_display()

    def check_serial(self):
        '''
        Should update self.state to a corresponding image in /images
        '''
        if self.ser.in_waiting > 0:
          data = self.ser.readline().decode('utf-8').rstrip()
          #print(data)
          if 'IMAGE' in data:
              state = data.split("IMAGE",1)[1][:3]
              self.state = state
          #time.sleep(0.01)
          self.update_display()


        self.master.after(1, self.check_serial)

    
    def update_display(self):
        self.canvas.delete("all")
        self.canvas.create_image(800/2, 490/2, image=self.gallery[self.state])

        
root = tk.Tk()
my_gui = GUI(root)
root.mainloop()


            
            
# tmp_sts = {'w':'AIM', 'a':'ANGLE', 's': 'POWER', 'd': 'SPIN', 'enter': 'MENU'}
# for c in tmp_sts.keys():
#     if keyboard.is_pressed(c):
#         print("can hear key")
#         self.state = tmp_sts[c]
#         self.update_display()
#         break