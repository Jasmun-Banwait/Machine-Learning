import tkinter as tk
import serial
import threading
import pandas as pd
from datetime import datetime
import scipy.signal
import ECG_Processsing
import numpy as np
# Define the COM port for the HC-05 module
bluetooth_port1 = 'COM13'  # Replace 'x' with the appropriate port number
bluetooth_port2 = 'COM11'
bluetooth1 = serial.Serial(bluetooth_port1, 9600, timeout=1) #Bluetooth connection banwidth decleration
bluetooth2 = serial.Serial(bluetooth_port2, 9600, timeout=1)
window = tk.Tk() #Initialize GUI
window.title("Arduino Bluetooth Communication")
ECG = []
TEMP = []
text_box1 = tk.Text(window, height=10, width=50) #Initialize Textbox 
text_box1.pack()
def ECGData_F(ECGData): #Create an array to store ECG data
    data_ECG = ECGData
    ECG.append(data_ECG)
def TempData(TempData):  #Create an array to store Temperature Data
    data_Temp = TempData
    TEMP.append(data_Temp)

# Function to continuously read data from the serial port
def read_data(): #Reads the serial port and decodes the data that comes through
    while True:
        data1 = bluetooth1.readline().decode('utf-8').strip()
        data2 = bluetooth2.readline().decode('utf-8').strip()
        ECGData_F(data1)
        TempData(data2)
        if data1: #Places the data at teh end of teh textbox
            text_box1.insert(tk.END, f"Received: {data1}\n")
            text_box1.see(tk.END)  # Auto-scroll to the bottom


def send_data(value): #Communicate with the bluetooth and writes to the arduino
    bluetooth1.write(value.encode('utf-8'))
    bluetooth2.write(value.encode('utf-8'))


def start_communication(): #Initiates the arduino if the iinput is 1
    send_data('1')

# Function to stop communication
def stop_communication(): #Stops communication
    send_data('0')


timestamp = datetime.now().strftime("%Y%m%d%H%M%S") #Initiates the date of incoming data
dir_path = "D:\Downloads"

dir_name = f"ECGData_{timestamp}.csv"

full_path = dir_path + dir_name
def Exit_1(): #Initiates the exit button on window
    stop_communication()
    bluetooth1.close()
    bluetooth2.close()
    DF1 = pd.DataFrame({"ECG Data": ECG, "Temperature Data": TEMP})
    DF1.to_csv(full_path, index=False)
    
    window.destroy()

# Create the Tkinter window


# Create GUI components
start_button = tk.Button(window, text="Start Communication", command=start_communication)
start_button.pack()

stop_button = tk.Button(window, text="Stop Communication", command=stop_communication)
stop_button.pack()

exit_button = tk.Button(window, text="Exit", command=Exit_1)
exit_button.pack(pady=10)

# Open the serial connection


# Create a thread to continuously read data from the serial port

read_thread = threading.Thread(target=read_data)
read_thread.start()

# Start the Tkinter main loop
window.mainloop()


