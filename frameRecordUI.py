import serial
import os
import tkinter as tk
from tkinter import scrolledtext
import threading
from imageio import get_writer
import pypylon.pylon as pylon
from tkinter import ttk
from tkinter import PhotoImage

# Serial Communication Setup
serial_port = 'COM17'  # Change this to the appropriate serial port
ser = serial.Serial(serial_port, 9600)

# Create the GUI window
root = tk.Tk()
root.title("LightStim Pulse Generator")

# Set window icon
icon = tk.PhotoImage(file="Images\AppIcon.png") # Load the image file from disk.
# Set it as the window icon.
root.iconphoto(True, icon)

# Create a frame for buttons
button_frame = tk.Frame(root)
button_frame.pack(side=tk.TOP, padx=10, pady=10)

# Create a scrolled text widget to display all received messages
all_text_widget = scrolledtext.ScrolledText(root, wrap=tk.WORD, height=15, width=60)  # Wider width
all_text_widget.pack(side=tk.LEFT, padx=10, pady=10)

# Create labels and text entry fields for Brightness, Frequency, On/Off Ratio, Time On, and Time Off
labels = ["Brightness (0-100%):", "Frequency (0-500Hz):", "Pulse Length (>0ms):", "Time On (sec):", "Time Off (sec):", "Text:"]
entries = [tk.Entry(root, width=30) for _ in range(len(labels))]

for i, label_text in enumerate(labels):
    label = tk.Label(root, text=label_text)
    label.pack(padx=10, pady=2, anchor="w")
    entries[i].pack(padx=10, pady=2)

# Create a text file for saving the data
current_directory = os.path.dirname(os.path.abspath(__file__))
relative_output_file_path = "output.txt"
output_file_path = os.path.join(current_directory, relative_output_file_path)
output_file = open(output_file_path, "a")

def send_data(event=None):
    values = [entry.get() for entry in entries]
    data = ' '.join(filter(None, values))

    if data:
        ser.write(b'6678\n')  # Send signal
        ser.write(data.encode('utf-8') + b'\n')

    # Clear text entry fields
    for entry in entries:
        entry.delete(0, tk.END)

# Create a button frame
button_frame = tk.Frame(root)
button_frame.pack(side=tk.TOP, padx=10, pady=10)

# Create a button to send data C:\Users\eadra\Desktop\School Stuff\Braingineering\LightStim\Images

sendButton_path = os.path.join(current_directory, "Images\sendButton.png")
sendIcon = PhotoImage(file = sendButton_path)
resizedsendIcon = sendIcon.subsample(10, 10)
send_button = tk.Button(button_frame, text="Send  ", image = resizedsendIcon, compound = "right", command=send_data)
send_button.grid(row=0, column=0, padx=5, pady=5)

# Create a button to clear text
def clear_text():
    all_text_widget.delete(1.0, tk.END)

clear_button = tk.Button(button_frame, text="Clear Text", command=clear_text)
clear_button.grid(row=0, column=1, padx=5, pady=5)

# Create a "Reset" button
def reset():
    ser.write(b'6789\n')

stopButton_path = os.path.join(current_directory, "Images\stopButton.png")
sendIcon = PhotoImage(file = stopButton_path)
resizedstopIcon = sendIcon.subsample(5, 5)
reset_button = tk.Button(button_frame, text="Reset", image = resizedstopIcon, compound = "right", command=reset)
reset_button.grid(row=2, column=1, padx=5)

# Create a "Start Pulsing" button
def start_pulsing():
    ser.write(b'6587\n')

startButton_path = os.path.join(current_directory, "Images\playButton.png")
startIcon = PhotoImage(file = startButton_path)
resizedstartIcon = startIcon.subsample(20, 20)
start_pulsing_button = tk.Button(button_frame, text="Start Pulsing", image = resizedstartIcon, compound = "right", command=start_pulsing)
start_pulsing_button.grid(row=2, column=0, padx=5, pady=5)

# Create a "Pause" button
def pause():
    ser.write(b'5432\n')

pauseButton_path = os.path.join(current_directory, "Images\pauseButton.png")
pauseIcon = PhotoImage(file = pauseButton_path)
resizedpauseIcon = pauseIcon.subsample(5, 5)
pause_button = tk.Button(button_frame, text="Pause", image = resizedpauseIcon, compound = "right", command=pause)
pause_button.grid(row=2, column=2, padx=5)

# Create an "Add to Queue" button
def add_to_queue():
    ser.write(b'9898\n')

add_to_queue_button = tk.Button(button_frame, text="Add to Queue", command=add_to_queue)
add_to_queue_button.grid(row=4, column=0, padx=5, pady=5)

# Create a "Remove From Queue" button
def remove_from_queue():
    ser.write(b'21212\n')

remove_from_queue_button = tk.Button(button_frame, text="Remove From Queue", command=remove_from_queue)
remove_from_queue_button.grid(row=4, column=1, padx=5)

# rem

# Camera Recorder App (left text box removed)
class CameraRecorderApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Camera Recorder")
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.fps = 5  # Hz
        self.time_to_record = 60  # seconds
        self.images_to_grab = self.fps * self.time_to_record
        self.recording = False

        self.create_ui()


    def create_ui(self):
        self.record_button = ttk.Button(self.root, text="Start Recording and Pulsing", command=self.toggle_record)
        self.record_button.pack(padx=10, pady=10)

        self.status_label = ttk.Label(self.root, text="")
        self.status_label.pack()


    def toggle_record(self):
        if not self.recording:
            self.start_record()
        else:
            self.stop_record()
# rem
    def start_record(self):
        self.recording = True
        self.record_button.config(text="Stop Recording & Pulsing")

        try:
            tlf = pylon.TlFactory.GetInstance()
            devices = tlf.EnumerateDevices()

            self.cam = pylon.InstantCamera(tlf.CreateDevice(devices[0]))
            self.cam.MaxNumBuffer = 60
            self.cam.Open()
            print("Using device ", self.cam.GetDeviceInfo().GetModelName())
            self.cam.AcquisitionFrameRate.SetValue(self.fps)

            self.writer = get_writer(
                'output-filename.avi',
                fps=self.fps,
                codec='libx264',
                quality=10,  # 0-10 higher val = better quality
                ffmpeg_params=[
                    '-preset',
                    'slow',
                    '-crf',
                    '15'
                ]
            )

            print(f"Recording {self.time_to_record} second video at {self.fps} fps")
            self.cam.StartGrabbingMax(self.images_to_grab, pylon.GrabStrategy_OneByOne)
            self.update_record_status()
            ser.write(b'6587\n')
            # Start a separate thread to record
            self.record_thread = threading.Thread(target=self.record)
            self.record_thread.start()
        except Exception as e:
            print(f"Error during camera initialization: {str(e)}")
# rem
    def record(self):
        try:
            while self.recording:
                with self.cam.RetrieveResult(1000, pylon.TimeoutHandling_ThrowException) as res:
                    if res.GrabSucceeded():
                        img = res.Array
                        self.writer.append_data(img)
                        print(res.BlockID, end='\r')
                        res.Release()
                    else:
                        print("Grab failed")

            self.writer.close()
            print("Recording stopped")
        except Exception as e:
            print(f"Error during recording: {str(e)}")

    def stop_record(self):
        self.recording = False
        self.record_thread.join()  # Wait for the recording thread to finish
        self.cam.StopGrabbing()
        self.cam.Close()
        ser.write(b'5432\n')
        print("Saving...", end=' ')
        self.record_button.config(text="Start Recording & Pulsing")
        self.update_record_status()

    def update_record_status(self):
        if self.recording:
            self.status_label.config(text="Recording...")
        else:
            self.status_label.config(text="Not Recording")

    def on_closing(self):
        if self.recording:
            self.stop_record()
        self.root.destroy()

# Function to toggle camera recording
camera_recording = False
camera_app = None

def toggle_camera_recording():
    global camera_recording, camera_app
    if not camera_recording:
        camera_app = CameraRecorderApp(root)
        camera_recording = True
    else:
        camera_app.on_closing()
        camera_recording = False

camera_button = tk.Button(button_frame, text="Toggle Camera Recording", command=toggle_camera_recording)
camera_button.grid(row=6, column=0, padx=5, pady=5)

# Start reading serial data (Your existing code)
def read_serial():
    while ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        # Display received data in all_text_widget with a newline
        all_text_widget.insert(tk.END, f"- {line}\n")
        all_text_widget.see(tk.END)

        # Check if "ON" or "OFF" is inside the line (case-sensitive)
        if "ON" in line or "OFF" in line:
            output_file.write(line + '\n')

    root.after(100, read_serial)

# Start reading serial data
read_serial()

# Bind the Enter key to the text entry field
entries[-1].bind('<Return>', send_data)

# Close the serial port and output file when the GUI is closed
def on_closing():
    if camera_recording:
        camera_app.on_closing()
    ser.close()
    output_file.close()
    root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)
root.mainloop()
