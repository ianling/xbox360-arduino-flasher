"""
xbox360-arduino-flasher
--Ian Ling (https://iancaling.com)

Dumps xbox360 NAND via an Arduino

"""
import serial
import struct
from math import floor
from sys import argv, exit
from time import time


# globals
SERIAL_DEVICE = "/dev/ttyACM0"
BAUD_RATE = 115200
DUMP_COMMAND = b"d"
FLASH_COMMAND = b"f"
FLASH_CONFIG_COMMAND = b"c"
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
    print(f"\nFinished at {end_time}")
    f.close()

# flash
elif argv[1] == "f":
    arduino.write(FLASH_COMMAND)
    # send length of file to write
    f = open(argv[2], 'rb')
    data = f.read()
    f.close()
    file_length = len(data)
    arduino.write(struct.pack(">I", file_length))
    # read back length from arduino
    expected_length = 11  # max length of a uint32_t cast to a string is 10 characters 
    buffer = b""
    while len(buffer) < expected_length:
        if len(buffer) > 0:
            print(repr(buffer))
        buffer += arduino.read(BAUD_RATE // 8)
    # throw out everything after the null byte (arduino sent a null terminated string)
    buffer = buffer[0:buffer.index(b"\x00")]
    if int(buffer) != file_length:
        print("Arduino sent back the wrong data length, aborting. No data was written, NAND is unaltered.")
        exit(1)
    print("Handshake successful, waiting for Arduino to request data...")
    
    # send data to arduino 1 page (528 bytes) at a time
    i = 0
    percent_done = 0
    arduino_ready_to_receive = False
    start_time = time()
    print(f"Started at {start_time}")
    while i < file_length:
        # wait for arduino to tell us it's ready to receive data
        while not arduino_ready_to_receive:
            if arduino.read(BAUD_RATE // 8) == b"\x00":
                arduino_ready_to_receive = True
        data_to_send = data[i:i + 528]
        arduino.write(data_to_send)
        
        i += 528
        arduino_ready_to_receive = False
        
        previous_percent_done = percent_done
        percent_done = floor(i/file_length*100)
        if percent_done - previous_percent_done > 0:
            time_elapsed = time() - start_time
            print(f"\r                                                         \r{percent_done}% -- {i // time_elapsed}Bps", end="")
    
    end_time = time()
    print(f"\nFinished at {end_time}")


# check flash config
elif argv[1] == "c":
    arduino.write(FLASH_CONFIG_COMMAND)
    expected_length = 4  # flash config is 4 bytes long
    buffer = b""
    while len(buffer) < expected_length:
        buffer += arduino.read(BAUD_RATE // 8)
    print(repr(buffer))
