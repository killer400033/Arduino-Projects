import tkinter as tk
import RPi.GPIO as GPIO
import time
import PIL
import math
import os
from PIL import Image
from PIL import ImageTk
from smbus import SMBus

addr = 0x8
bus = SMBus(1)

master = tk.Tk()
master.title("LATHE TEST")
master.geometry("1280x1024")
threadphoto = tk.PhotoImage(file = "screw.png")

#Random Variables
flash_cycle = False
last_time = 0.0
rpm_time = 0
rpm_num = 0
mode = 0
curr_RPM = 0.0
RPM_arr = ["0", "0", "0", "0"]
count = 0
feed_pitch = 200
data = [0, 0, 0, 0, 0, 0, 0, 0]
data_c = 0
depth_n = ["0", ".", "0", "0"];
depth_n_temp = ""
depth_temp = "";

#Num Stringvars
feed_pitch_c = tk.StringVar()
rpm_count = tk.StringVar()
depth_count = tk.StringVar()
pitch_count = tk.StringVar()

#Name Stringvars
feed_pitch_n = tk.StringVar()

#StringVar initializing
feed_pitch_n.set("Threading Pitch")
temp = ['0','.','0','0']
feed_pitch_c.set(''.join(temp))
depth_count.set("0.00")
pitch_count.set("0.00")

MATRIX = [ ["1", "2", "3", "clear"],
           ["4", "5", "6", "B"],
           ["7", "8", "9", "mode"],
           ["*", "0", "#", "set"] ]

COL = [10, 8, 12, 16]
ROW = [7, 15, 13, 11]

    
Metric_Label = tk.Label(master, text = "Metric", bg = "green3", fg = "white", font = ("Helvetica", 40), relief = "solid", padx = 110)
Metric_Label.place(relx = 0.02, rely = 0.02)
Imperial_Label = tk.Label(master, text = "Imperial", bg = "grey", fg = "black", font = ("Helvetica", 40), relief = "solid", padx = 100)
Imperial_Label.place(relx = 0.36, rely = 0.02)
Feed_Label = tk.Label(master, text = "Power Feed", bg = "grey", fg = "black", font = ("Helvetica", 40), relief = "solid", padx = 20)
Feed_Label.place(relx = 0.72, rely = 0.02)

thread_Label = tk.Label(master, textvariable = feed_pitch_n, bg = "light grey", fg = "black", font = ("Helvetica", 30), padx = 150)
thread_Label.place(relx = 0.02, rely = 0.15)

pitch_num_Label = tk.Label(master, textvariable = feed_pitch_c, bg = "grey25", fg = "red3", font = ("Helvetica", 200), relief = "solid", padx = 30)
pitch_num_Label.place(relx = 0.02, rely = 0.22)

rpm_Label = tk.Label(master, text = "RPM", bg = "light grey", fg = "black", font = ("Helvetica", 30), padx = 240)
rpm_Label.place(relx = 0.02, rely = 0.56)

rpm_num_Label = tk.Label(master, textvariable = rpm_count, bg = "grey25", fg = "red3", font = ("Helvetica", 180), relief = "solid", padx = 20)
rpm_num_Label.place(relx = 0.02, rely = 0.65)

depth_Label = tk.Label(master, textvariable = depth_count, bg = "light grey", fg = "black", font = ("Helvetica", 30), relief = "solid", padx = 20)
depth_Label.place(relx = 0.72, rely = 0.18)

pitch_Label = tk.Label(master, textvariable = pitch_count, bg = "light grey", fg = "black", font = ("Helvetica", 30), relief = "solid", padx = 20)
pitch_Label.place(relx = 0.84, rely = 0.32)

thread_pic = tk.Label(master, image = threadphoto)
thread_pic.place(relx = 0.58, rely = 0.24)



GPIO.setmode(GPIO.BOARD)

GPIO.setup(19, GPIO.IN, pull_up_down = GPIO.PUD_UP)
GPIO.setup(22, GPIO.IN, pull_up_down = GPIO.PUD_OFF)

for j in range(4):
    GPIO.setup(COL[j], GPIO.OUT)
    GPIO.output(COL[j], 1)

for i in range(4):
    GPIO.setup(ROW[i], GPIO.IN, pull_up_down = GPIO.PUD_UP)
    
def encoder_click_1(channel):
    global rpm_num
    rpm_num += 1
    
GPIO.add_event_detect(19, GPIO.FALLING, callback = encoder_click_1)

while(True):
        if(GPIO.input(22) == 0):
            os.system("sudo shutdown -h now")
            
        
        if(time.time() - rpm_time > 0.5):
            curr_RPM = 60 / 17.8 * rpm_num / (time.time() - rpm_time)
            for x in range(4):
                RPM_arr[x] = str(math.floor(curr_RPM / 10 ** (3-x)))
                curr_RPM -= ((math.floor(curr_RPM / 10 ** (3-x))) * 10 ** (3-x))
            rpm_count.set(''.join(RPM_arr))
            rpm_time = time.time()
            rpm_num = 0
        
        
        if((time.time() - last_time) > 0.5):
            if((mode == 0 and count < 4) or (mode != 0 and count < 3)):
                if(flash_cycle == True):
                    temp[count] = '  '
                    feed_pitch_c.set(''.join(temp))
                    flash_cycle = False
                else:
                    temp[count] = '0'
                    feed_pitch_c.set(''.join(temp))
                    flash_cycle = True
            last_time = time.time()
        master.update_idletasks()
        for j in range(4):
            GPIO.output(COL[j], 0)

            for i in range(4):
                
                if (GPIO.input(ROW[i]) == 0):
                    if(i != 3 and j != 3 or MATRIX[i][j] == "0"):
                        if(count < 4 and mode == 0 or count < 3 and mode != 0):
                            temp[count] = MATRIX[i][j]
                            data[data_c] = int(MATRIX[i][j])
                            if(count == 0 and mode == 0):
                                count += 1
                            feed_pitch_c.set(''.join(temp))
                            count += 1
                            data_c += 1
                            
                    elif(MATRIX[i][j] == "clear"):
                        count = 0
                        data_c = 0
                        if(mode != 0):
                            temp = ['0','0','0']
                        else:
                            temp = ['0','.','0','0']
                        feed_pitch_c.set(''.join(temp))
                        
                    elif(MATRIX[i][j] == "mode"):
                        if(mode < 2):
                            mode += 1
                        else:
                            mode = 0
                        if(mode == 0):
                            feed_pitch_n.set("Threading Pitch")
                            Metric_Label.config(bg = "green3", fg = "white")
                            Feed_Label.config(bg = "grey", fg = "black")
                            temp = ['0','.','0','0']
                            feed_pitch_c.set(''.join(temp))
                            count = 0
                            data_c = 0
                        elif(mode == 1):
                            Imperial_Label.config(bg = "green3", fg = "white")
                            Metric_Label.config(bg = "grey", fg = "black")
                            temp = ['0','0','0']
                            feed_pitch_c.set(''.join(temp))
                            data_c = 0
                            count = 0
                        elif(mode == 2):
                            feed_pitch_n.set("  Feed Rate  ")
                            Feed_Label.config(bg = "green3", fg = "white")
                            Imperial_Label.config(bg = "grey", fg = "black")
                            temp = ['0','0','0']
                            feed_pitch_c.set(''.join(temp))
                            data_c = 0
                            count = 0
                            
                    elif(MATRIX[i][j] == "set"):
                        try:
                            if(mode == 2):
                                depth_count.set("0.00")
                                pitch_count.set("0.00")
                                data[7] = 2
                                bus.write_i2c_block_data(0x8, 0, data)
                            elif(mode == 0):
                                pitch_count.set(''.join(temp))
                                depth_n_temp = ''.join([temp[0], temp[2], temp[3]])
                                depth_temp = str(int(depth_n_temp) * 0.00866025)
                                for i in range(4):
                                    depth_n[i] = depth_temp[i]
                                
                                depth_count.set(''.join(depth_n))
                                data[7] = 0
                                bus.write_i2c_block_data(0x8, 0, data)
                            elif(mode == 1):
                                pitch_count.set("0.00")
                                depth_n_temp = ''.join([temp[0], temp[1], temp[2]])
                                depth_temp = str(25.4 / int(depth_n_temp) * 0.866025)
                                for i in range(4):
                                    depth_n[i] = depth_temp[i]
                                
                                depth_count.set(''.join(depth_n))
                                data[7] = 1
                                bus.write_i2c_block_data(0x8, 0, data)
                        except:
                            pass
                    while (GPIO.input(ROW[i]) == 0):
                        pass
                    
            GPIO.output(COL[j], 1)
            