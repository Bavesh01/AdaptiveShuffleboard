import tkinter as tk
import keyboard
import serial
import time


class TextDisplayGUI:
    def __init__(self, master):
        self.master = master
        self.master.title("Dynamic Text Display")

        self.text = tk.StringVar()  # This variable will hold the text to display
        self.text.set("Press any key...")

        # Create a canvas with a white background
        self.canvas = tk.Canvas(self.master, width=800, height=450, bg='white')
        self.canvas.pack()

        try:
            self.ser = serial.Serial('/dev/ttyACM0', 2000000)
        except:
            self.ser = serial.Serial('/dev/ttyACM1', 2000000)


        
        time.sleep(2)


        # Place text in the middle of the canvas
        self.text_id = self.canvas.create_text(400, 225, text=self.text.get(), font=('Helvetica', 20))

        self.master.after(100, self.check_serial)
        self.update_display("hi")

    def check_serial(self):
        '''
        Should update self.state to a corresponding image in /images
        '''
        print("heya")

        # if self.ser.in_waiting > 0:
        #     data = self.ser.readline().decode('utf-8').rstrip()
            # FIND NATURE OF DATA
        
        if self.ser.in_waiting > 0:
          data = self.ser.read().decode('utf-8').rstrip()
          print(data)
          time.sleep(0.1)
          self.update_display(data)

        # tmp_sts = {'w':'AIM', 'a':'ANGLE', 's': 'POWER', 'd': 'SPIN', 'enter': 'MENU'}
        # for c in tmp_sts.keys():
        #     if keyboard.is_pressed(c):
        #         print("can hear key")
        #         self.state = tmp_sts[c]
        #         self.update_display(c)
        #         break
        self.master.after(100, self.check_serial)

    def update_display(self, current_text):
        self.text.set(current_text)  # Update the text variable
        self.canvas.itemconfigure(self.text_id, text=self.text.get())  # Update the canvas text

        
        # self.canvas.delete("all")
        # self.canvas.create_image(400, 300, image=self.gallery[self.state])


    # def key_pressed(self, event):
    #     # Update the text based on the key pressed
    #     current_text = self.text.get()
    #     if event.char.isprintable():
    #         current_text += event.char  # Add the character to the current text
    #     elif event.keysym == 'BackSpace':
    #         current_text = current_text[:-1]  # Remove the last character
    #     elif event.keysym == 'Return':
    #         current_text += '\n'  # Add newline for Enter key
    #     elif event.keysym == 'Space':
    #         current_text += ' '  # Add space for Spacebar

    #     self.text.set(current_text)  # Update the text variable
    #     self.canvas.itemconfigure(self.text_id, text=self.text.get())  # Update the canvas text

if __name__ == "__main__":
    root = tk.Tk()
    gui = TextDisplayGUI(root)
    root.mainloop()
