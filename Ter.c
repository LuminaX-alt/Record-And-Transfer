python3 host_audio_server.py
python3 robot_audio_client.py
pip install pyaudio
sudo apt-get install portaudio19-dev python3-pyaudio
//Robot Audio Recorder (Client) + Host Receiver (Server)
  # robot_audio_client.py
import socket
import pyaudio

# Audio configuration
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000
CHUNK = 1024  # Number of audio frames per buffer

# Host server details (replace with your host IP)
HOST = '192.168.1.100'
PORT = 50007

def main():
    p = pyaudio.PyAudio()
    
    # Open audio stream (input from mic)
    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)
    
    # Create TCP socket and connect to host
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print(f"Connecting to server {HOST}:{PORT}...")
    sock.connect((HOST, PORT))
    print("Connected. Streaming audio... Press Ctrl+C to stop.")
    
    try:
        while True:
            # Read audio data from microphone
            data = stream.read(CHUNK)
            # Send data to host
            sock.sendall(data)
    except KeyboardInterrupt:
        print("\nStreaming stopped by user.")
    finally:
        stream.stop_stream()
        stream.close()
        p.terminate()
        sock.close()

if __name__ == "__main__":
    main()

//Host Side (TCP Server + Save Audio to WAV)
# host_audio_server.py
import socket
import wave

HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 50007

# Audio parameters (must match client)
CHANNELS = 1
SAMPLE_WIDTH = 2  # 2 bytes for paInt16
RATE = 16000
CHUNK = 1024

def main():
    # Setup WAV file for output
    wf = wave.open("received_audio.wav", 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(SAMPLE_WIDTH)
    wf.setframerate(RATE)

    # Setup TCP server socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((HOST, PORT))
    sock.listen(1)
    print(f"Listening for incoming audio connections on {HOST}:{PORT}...")

    conn, addr = sock.accept()
    print(f"Connected by {addr}")

    try:
        while True:
            data = conn.recv(CHUNK)
            if not data:
                break
            wf.writeframes(data)
    except KeyboardInterrupt:
        print("\nServer stopped by user.")
    finally:
        wf.close()
        conn.close()
        sock.close()
        print("Connection closed and audio file saved as received_audio.wav")

if __name__ == "__main__":
    main()

