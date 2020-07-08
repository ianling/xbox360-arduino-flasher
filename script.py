"""
xbox360-arduino-flasher
--Ian Ling (https://iancaling.com)

Dumps xbox360 NAND via an Arduino

"""
import serial
from time import time


# globals
SERIAL_DEVICE = "/dev/ttyACM0"
BAUD_RATE = 115200
DUMP_COMMAND = b"d"
FLASH_COMMAND = b"f"
OUT_FILE = "dump.bin"


arduino = serial.Serial(SERIAL_DEVICE, BAUD_RATE, timeout=1)
f = open("dump.bin", "wb")
arduino.write(DUMP_COMMAND)
expected_length = 17301504
i = 0

start_time = time()
print(f"Started at {start_time}")

while i < expected_length:
    data = arduino.read()
    f.write(data)
    i += 1
    if i % 173015 == 0:
        print(f"{i/expected_length*100}%")

end_time = time()
print(f"Finished at {end_time}")
f.close()
arduino.close()
