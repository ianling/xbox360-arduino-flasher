# xbox360-arduino-flasher
Xbox 360 NAND reader/writer for Arduino

Based on:

* G33KatWork: https://github.com/G33KatWork/XBox-360-AVR-flasher 
* walczakp: https://github.com/walczakp/xbox360-rpi-flasher

See XSPI.h for pin assignments. Dumps NAND to serial at 115200bps (8N1).

Instructions
------------

* Ensure you have at least Python 3.6, and have installed the pyserial package
* Wire everything up as you would when using any other tool, but follow the pin assignments in XSPI.h
* Connect the power cable to the Xbox 360, but do not power it on
* Connect the Arduino to your computer via USB
* Flash the sketch xbox360-arduino-flasher.ino to your Arduino
* Run script.py to dump the NAND (dumps to dump.bin by default)

I have tested with an Arduino Leonardo-compatible board and a Falcon Xbox 360.
This setup takes about 17 minutes to dump the NAND.

The sketch makes use of the digitalWriteFast library to improve data transfer speeds.
You should verify that your Arduino is compatible with this library.
If your board is not compatible and you want to switch back to the Arduino standard library,
simply remove all instances of the word "Fast" from the code and delete digitalWriteFast.h

TODO
----

* Writing (this only reads the NAND right now)
