import tkinter as tk
import time
import PIL
import math
from PIL import Image
from PIL import ImageTk

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
            