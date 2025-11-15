import serial
import numpy as np
import matplotlib.pyplot as plt
import wave
import time

# ======================
# User settings
# ======================
UART_PORT = '/dev/ttyUSB0'  # Change to your port
BAUD_RATE = 921600
SAMPLE_RATE = 44100         # INMP441 is configured for 16kHz
BUFFER_SIZE = 1024          # Number of samples per read
ROLLING_SIZE = 4096         # Number of samples to display/keep
DURATION_SEC = 5            # Total recording duration (can be ignored for continuous plotting)
# ======================

# Open UART
ser = serial.Serial(UART_PORT, BAUD_RATE)

# Prepare rolling buffer
rolling_buffer = np.zeros(ROLLING_SIZE, dtype=np.int16)

# Setup plotting
plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(rolling_buffer)
ax.set_ylim(-32768, 32767)
# ax.set_xlim(0, ROLLING_SIZE)
ax.set_xlabel("Sample Index")
ax.set_ylabel("Amplitude")
ax.set_title("Live Audio Waveform (Scrolling)")

# Prepare for WAV saving
all_samples = []

start_time = time.time()
try:
    while True:
    # Read raw data from UART
        raw = ser.read(BUFFER_SIZE * 4)  # 1024 samples * 4 bytes (int32)
        # samples = np.frombuffer(raw, dtype=np.int32)
        samples = np.frombuffer(raw, dtype='<i4')
        # Convert 32-bit left-justified to 16-bit PCM
        pcm_16 = (samples >> 16).astype(np.int16)
        # pcm_16 = (samples >> 8).astype(np.int16)
        # pcm_16 = (samples >> 14).astype(np.int16)
        # Append to rolling buffer (scrolling)
        # rolling_buffer[:-BUFFER_SIZE] = rolling_buffer[BUFFER_SIZE:]
        # rolling_buffer[-BUFFER_SIZE:] = pcm_16
        # line.set_ydata(rolling_buffer)
        # fig.canvas.draw()
        # fig.canvas.flush_events()

        # Keep all samples for WAV saving
        all_samples.extend(pcm_16)

        # Optional: stop after DURATION_SEC seconds
        if time.time() - start_time > DURATION_SEC:
            break

finally:
    ser.close()

# Save WAV
all_samples = np.array(all_samples, dtype=np.int16)
with wave.open("recorded_audio.wav", "wb") as wf:
    wf.setnchannels(1)             # Mono
    wf.setsampwidth(2)             # 16-bit
    wf.setframerate(SAMPLE_RATE)
    wf.writeframes(all_samples.tobytes())

print("Recording saved as recorded_audio.wav")


# import serial
# import numpy as np
# import wave
# import matplotlib.pyplot as plt
# # replace with your port
# ser = serial.Serial('/dev/ttyUSB0', 921600)

# # receive 16-bit PCM, mono
# pcm_samples = []
# plt.ion()  # interactive mode ON

# fig, ax = plt.subplots()
# line, = ax.plot([], [])
# ax.set_ylim(-32768, 32767)
# ax.set_xlim(0, 1024)  # adjust to buffer size

# while(1):
#     try:
#         while len(pcm_samples) < 16000:  # 1 second of audio at 16 kHz
#             raw = ser.read(1024 * 4)  # 1024 * 32-bit
#             samples = np.frombuffer(raw, dtype=np.int32)
#             pcm_samples.extend(samples)
#     finally:
#         ser.close()

#     # convert to numpy array
#     pcm_samples = np.array(pcm_samples, dtype=np.int32)
#     pcm_16 = (pcm_samples >> 16).astype(np.int16)
#     line.set_ydata(pcm_16)
#     line.set_xdata(np.arange(len(pcm_16)))
#     fig.canvas.draw()
#     fig.canvas.flush_events()
#     # print(pcm_16[500:600])

# # save as WAV
# with wave.open("audio.wav", "wb") as wf:
#     wf.setnchannels(1)
#     wf.setsampwidth(2) # 16-bit
#     wf.setframerate(16000)
#     pcm_16 = (pcm_samples >> 16).astype(np.int16)
#     wf.writeframes(pcm_16.tobytes())
#     # wf.writeframes(pcm_samples.tobytes())

# print("Saved audio.wav")
