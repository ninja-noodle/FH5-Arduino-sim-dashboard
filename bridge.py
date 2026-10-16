import socket
import struct
import time
import serial

ARDUINO_PORT = "COM3"
BAUD_RATE = 9600
UDP_IP = "127.0.0.1"
UDP_PORT = 5300

print("Connecting to Arduino on", ARDUINO_PORT)
arduino = serial.Serial(ARDUINO_PORT, BAUD_RATE, timeout=0)
time.sleep(2)
print("Arduino connected!")

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
sock.setblocking(False)
print(f"listening for FH5 telemetry on port {UDP_PORT}...")

last_gear = None
blink_state = False
last_blink_time = time.time()

try:
  while True:
    try:
      data, addr = sock.recvfrom(2048)

      # Forza Horizon 5 Dash packet length is 324 bytes
      if len(data) >= 324:
        engine_max_rpm = struct.unpack_from("<f", data, 8)[0]
        current_rpm = struct.unpack_from("<f", data, 16)[0]
        gear = data[319]  # Correct verified gear index

        # 1. Send gear update to Arduino if it changes
        if gear != last_gear:
          print(f"Current Gear: {gear}")
          arduino.write(f"G{gear}\n".encode("utf-8"))
          last_gear = gear

        # 2. Shift light blinking logic (Triggers at 88% of max RPM)
        is_shift_time = (
            engine_max_rpm > 0 and current_rpm >= (engine_max_rpm * 0.88)
        )

        if is_shift_time:
          if time.time() - last_blink_time > 0.15:
            blink_state = not blink_state
            state_char = "1" if blink_state else "0"
            arduino.write(f"X{state_char}\n".encode("utf-8"))
            last_blink_time = time.time()
        else:
          if blink_state:
            arduino.write(b"X0\n")
            blink_state = False
    except BlockingIOError:
      pass

    time.sleep(0.001)

except KeyboardInterrupt:
  print("\nClosing bridge.")
  sock.close()
  arduino.close()