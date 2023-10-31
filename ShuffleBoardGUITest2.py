import io
from tkinter import *
from tkinter import messagebox
from PIL import Image, ImageTk
import serial
import paramiko
import matplotlib.pyplot as plt
import numpy as np

LARGEFONT = ("Verdana", 35)

class GUI(Tk):
    def __init__(self, *args, **kwargs):
        Tk.__init__(self, *args, **kwargs)
        self.title = "Shuffleboard Homescreen"
        self.attributes('-fullscreen', True)
 
        #self.ser = serial.Serial('COM3', 9600)
        #self.ser.flush()

        container = Frame(self, bg='navy').pack(expand=True, fill=BOTH)

        self.frames = {}

        for F in (HomeScreen, AimConfirm):
            frame = F(container,self)
            self.frames[F] = frame

        self.show_frame(HomeScreen)

    def show_frame(self, cont):
        frame = self.frames[cont]
        frame.tkraise()

class HomeScreen(Frame):
    def __init__(self, parent, controller):
        Frame.__init__(self, parent)
        
        label = Label(self, text="HomeScreen", font = LARGEFONT, fg='black')
        label.pack()
    
class AimConfirm(Frame):
    def __init__(self, parent, controller):
        Frame.__init__(self, parent)
        
        label = Label(self, text="AimConfirm", font = LARGEFONT)
        label.pack()


ShuffleboardGUI = GUI()
ShuffleboardGUI.mainloop()