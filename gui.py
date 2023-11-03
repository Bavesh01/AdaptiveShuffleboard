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

        self.state = 'MENU'

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
        }
        for i in range(1,5):
            self.states[str(200+i)] = 'SPIN_MODE_RIGHT' + str(i)
            self.states[str(200-i)] = 'SPIN_MODE_LEFT' + str(i)
        for i in range(1,17):
            self.states[str(400+i)] = 'POWER_MODE_' + str(i)
        print(self.states)

        self.gallery = {s:ImageTk.PhotoImage(Image.open(f'images/{s}.png'))
                   for s in self.states.keys()}


        try:
            self.ser = serial.Serial('/dev/ttyACM0', 2000000)
        except:
            self.ser = serial.Serial('/dev/ttyACM1', 2000000)

        time.sleep(2)


        self.canvas = tk.Canvas(self.master, width=800, height=450)
        self.canvas.pack()

        self.master.after(100, self.check_serial)
        self.update_display()

    def check_serial(self):
        '''
        Should update self.state to a corresponding image in /images
        '''
        print("heya")

        # if self.ser.in_waiting > 0:
        #     data = self.ser.readline().decode('utf-8').rstrip()
        
        if self.ser.in_waiting > 0:
          data = self.ser.read().decode('utf-8').rstrip()
          print(data)
          if 'IMAGE' in data:
              state = data.split("IMAGE",1)[1][:3]
              self.state = state
          time.sleep(0.01)
          self.update_display()

            
            
        # tmp_sts = {'w':'AIM', 'a':'ANGLE', 's': 'POWER', 'd': 'SPIN', 'enter': 'MENU'}
        # for c in tmp_sts.keys():
        #     if keyboard.is_pressed(c):
        #         print("can hear key")
        #         self.state = tmp_sts[c]
        #         self.update_display()
        #         break
        self.master.after(100, self.check_serial)

    
    def update_display(self):
        self.canvas.delete("all")
        self.canvas.create_image(400, 225, image=self.gallery[self.state])

        
root = tk.Tk()
my_gui = GUI(root)
root.mainloop()
