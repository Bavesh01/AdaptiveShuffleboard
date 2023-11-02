import io
import tkinter as tk
from PIL import Image, ImageTk
import serial
import keyboard

class GUI:
    def __init__(self, master):
        self.master = master
        master.title("Shuffleboard Homescreen")

        self.state = 'MENU'

        self.states = ['MENU', 'AIM', 'SPIN', 'ANGLE', 'POWER']
        self.gallery = {s:ImageTk.PhotoImage(Image.open(f'images/{s}.png'))
                   for s in self.states}

        
        self.ser = serial.Serial('COM7', 9600)


        self.canvas = tk.Canvas(self.master, width=800, height=600)
        self.canvas.pack()

        self.master.after(100, self.check_serial)
        self.update_display()

    def check_serial(self):
        '''
        Should update self.state to a corresponding image in /images
        '''
        print("heya")

        if self.ser.in_waiting > 0:
            data = self.ser.readline().decode('utf-8').rstrip()
            # FIND NATURE OF DATA
            
        tmp_sts = {'w':'AIM', 'a':'ANGLE', 's': 'POWER', 'd': 'SPIN', 'enter': 'MENU'}
        for c in tmp_sts.keys():
            if keyboard.is_pressed(c):
                print("can hear key")
                self.state = tmp_sts[c]
                self.update_display()
                break
        self.master.after(100, self.check_serial)

    
    def update_display(self):
        self.canvas.delete("all")
        self.canvas.create_image(400, 300, image=self.gallery[self.state])

        
root = tk.Tk()
my_gui = GUI(root)
root.mainloop()
