import io
import tkinter as tk
from PIL import Image, ImageTk
import serial

class GUI:
    def __init__(self, master):
        self.master = master
        master.title("Shuffleboard Homescreen")

        self.state = 'MENU'

        states = ['MENU', 'AIM', 'SPIN', 'ANGLE', 'POWER']
        gallery = {s:ImageTk.PhotoImage(Image.open(s+'.jpg'))
                   for s in states}
        
        self.ser = serial.Serial('COM3', 9600)


        self.canvas = tk.Canvas(self.root, width=800, height=600)
        self.canvas.pack()

        self.root.after(100, self.check_serial)
        self.update_display()
        


        # self.label_username = tk.Label(master, text="Username:")
        # self.label_password = tk.Label(master, text="Password:")

        # self.entry_username = tk.Entry(master)
        # self.entry_password = tk.Entry(master, show="*")

        # self.label_username.grid(row=0, sticky=tk.E)
        # self.label_password.grid(row=1, sticky=tk.E)
        # self.entry_username.grid(row=0, column=1)
        # self.entry_password.grid(row=1, column=1)

        # self.logbtn = tk.Button(master, text="Log In", command=self._ssh_login)
        # self.logbtn.grid(columnspan=2)

        # self.username = ''
        # self.password = ''

    def check_serial():
        


    def _ssh_login(self):
        # connect = False
        # while not connect:
        #     self.__init__(self.master)
        #     try:
        #         connect=True
        #     except:
        #         tk.messagebox.showerror("Error", "Unable to connect :(")

        self.username = self.entry_username.get()
        self.password = self.entry_password.get()
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect('erisone.partners.org', username=self.username, password=self.password)

        stdin, stdout, stderr = ssh.exec_command('ls')
        file_list = stdout.readlines()

        stdin, stdout, stderr = ssh.exec_command('pwd')
        cwd = stdout.read().decode().strip()

        ssh.close()

        result_window = tk.Toplevel(self.master)
        result_window.title("Directory Listing")

        label_cwd = tk.Label(result_window, text=f"Current Working Directory: {cwd}")
        label_cwd.pack()

        # label_files = tk.Label(result_window, text="Files in home directory:")
        # label_files.pack()

        # text_files = tk.Text(result_window)
        # text_files.pack(side=tk.LEFT)
        # print(file_list)
        # for file_name in file_list:
        #     text_files.insert(tk.END, file_name)
        # text_files.config(state=tk.DISABLED)

        coeff_frame = tk.Frame(result_window)
        coeff_frame.pack()
                
        check_dict = {insight:tk.BooleanVar() for insight in 
                      ['ap','dissolution','fear','misery','disgust','desire_for_deliverance','reobservation','equanimity']}
        checkbox_frame = tk.Frame(coeff_frame)
        checkbox_frame.pack(side=tk.TOP)

        for label in check_dict.keys():
            checkbox = tk.Checkbutton(checkbox_frame, text=label, variable=check_dict[label])
            checkbox.pack(side=tk.TOP)
        
        pltbutton = tk.Button(coeff_frame, text="Show me plot", command=lambda: self.create_plot(check_dict))
        pltbutton.pack(side=tk.BOTTOM)
        self.b_entry = tk.Entry(coeff_frame)
        self.b_entry.pack(side=tk.BOTTOM)
        b_label = tk.Label(coeff_frame, text="bins:")
        b_label.pack(side=tk.BOTTOM)
        self.a_entry = tk.Entry(coeff_frame)
        self.a_entry.pack(side=tk.BOTTOM)
        q_label = tk.Label(coeff_frame, text="insight: ")
        q_label.pack(side=tk.BOTTOM)

    def create_plot(self, check_dict):

        result_window2 = tk.Toplevel(self.master)
        single = True
        if single:
            bins = int(self.b_entry.get())
            insight = str(self.a_entry.get())
        else:
            insights = [insight for insight in check_dict.keys() if check_dict[insight].get()]
            insights = ['ap']
            insight = 'ap'
            bins = int(self.b_entry.get())
        

        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect('erisone.partners.org', username=self.username, password=self.password)

        insight_data = {}
        data = []
        path='/data/mgh-meditation/connectome_harmonics/insight/chap_100_test2/chap/sub-ID01/func/ght'

        spectra = 'energyspectra'
        command = f'ls {path}/{spectra}/'
        stdin, stdout, stderr = ssh.exec_command(command)
        file_list = [file[:-1] for file in stdout.readlines()]

        if not single:
            sftp = ssh.open_sftp()
            for insight in insights:
                data = []
                for file in file_list:
                    if insight in file and 'mean' in file and file.endswith('.npy'):
                        with sftp.open(f'{path}/{spectra}/{file}') as f:
                            data.append(np.load(io.BytesIO(f.read())))
                insight_data[insight] = data
                print(data)
                    
            sftp.close()
            ssh.close()
            width = 100//bins
            remainder = 100%bins
            nums = range(0,100,width) # Hard coded :( = mean_data.shape[0]


            # mean_data = np.mean(insight_data['ap'], axis=0)
            # width = mean_data.shape[0]//bins
            # remainder =  mean_data.shape[0]%bins
            # output=[np.mean(mean_data[i:i+width]) for i in range(0, mean_data.shape[0]-width,width)]
            # output.append(np.mean(mean_data[-(width+remainder):]))

            mean_datas = {insight:np.mean(insight_data[insight], axis=0) for insight in insight_data.keys()}
            output={insight:[np.mean(mean_data[i:i+width]) for i in range(0, 100-width,width)] + (np.mean(mean_data[-(width+remainder):])) for insight, mean_data in mean_datas.items()}
                
            print(nums)
            print(output)

            fig, ax = plt.subplots()
            offset = 0
            title_str = f'Energy dist. ({bins} bins): '
            for insight in insights:
                ax.bar([num+offset for num in nums], output[insight], width=width*0.9, align='edge', label=insight)
                offset += width*0.15
                title_str += f'{insight} '
            ax.legend(loc='upper right')
            ax.set_xlabel("Harmonics")
            ax.set_ylabel("Energy Value")
            ax.set_title(title_str)
        else:
            sftp = ssh.open_sftp()
            for file in file_list:
                if insight in file and 'mean' in file and file.endswith('.npy'):
                    with sftp.open(f'{path}/{spectra}/{file}') as f:
                        data.append(np.load(io.BytesIO(f.read())))
            sftp.close()
            ssh.close()
            mean_data = np.mean(data, axis=0)
            width = mean_data.shape[0]//bins
            remainder =  mean_data.shape[0]%bins
            output=[np.mean(mean_data[i:i+width]) for i in range(0, mean_data.shape[0]-width,width)]
            output.append(np.mean(mean_data[-(width+remainder):]))
            nums = range(0,len(mean_data),width)

            print(len(output))
            print(len(nums))

            fig, ax = plt.subplots()
            ax.bar(nums, output, width=width, align='edge')
            ax.set_xlabel("Harmonics")
            ax.set_ylabel("Energy Value")
            ax.set_title(f'Energy dist. {insight}')

        canvas = FigureCanvasTkAgg(fig, result_window2)
        canvas.draw()
        canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        toolbar = NavigationToolbar2Tk(canvas, result_window2)
        toolbar.update()
        canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

root = tk.Tk()
my_gui = GUI(root)
root.mainloop()
