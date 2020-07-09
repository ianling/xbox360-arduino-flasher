"""
xbox360-arduino-flasher
--Ian Ling (https://iancaling.com)

Dumps xbox360 NAND via an Arduino

"""
import serial
from math import floor
from sys import argv
from time import time


# globals
SERIAL_DEVICE = "/dev/ttyACM0"
BAUD_RATE = 115200
DUMP_COMMAND = b"d"
FLASH_COMMAND = b"f"
FLASH_CONFIG_COMMAND = b"c"
TEST_COMMAND = b"t"
OUT_FILE = "dump.bin"


arduino = serial.Serial(SERIAL_DEVICE, BAUD_RATE, timeout=0)


# dump
if argv[1] == "d":
    f = open("dump.bin", "wb")
    arduino.write(DUMP_COMMAND)
    expected_length = 17301504
    percent_done = 0
    i = 0

    start_time = time()
    print(f"Started at {start_time}")

    while i < expected_length:
        data = arduino.read(BAUD_RATE // 8)
        i += len(data)
        f.write(data)
        previous_percent_done = percent_done
        percent_done = floor(i/expected_length*100)
        if percent_done - previous_percent_done > 0:
            time_elapsed = time() - start_time
            print(f"\r                                                         \r{percent_done}% -- {i // time_elapsed}Bps", end="")

    end_time = time()
    print(f"Finished at {end_time}")
    f.close()

# flash
elif argv[1] == "f":
    pass  # TODO

# check flash config
elif argv[1] == "c":
    arduino.write(FLASH_CONFIG_COMMAND)
    expected_length = 4  # flash config is 4 bytes long
    buffer = b""
    while len(buffer) < expected_length:
        buffer += arduino.read(BAUD_RATE // 8)
    print(repr(buffer))

# test raw serial mode
elif argv[1] == "t":
    arduino.write(TEST_COMMAND)
    response = arduino.read()
    print(repr(response))

arduino.close()
