import argparse
import serial
import sys
import os
from xmodem import XMODEM

# Requires PySerial - pip install pyserial
# Requires xmodem   - pip install xmodem


# Declare function calls
def error(msg):
    print ('\r\nError: %s' % msg)
    sys.exit(1)

def getc(size, timeout=1):
    return port.read(size) or None

def putc(data, timeout=1):
    return port.write(data)  # note that this ignores the timeout

def readPort(msg, err):
    while 1:
            response = port.readline()
            print(response)
            if b'Error' in response:
                # Error, Quit
                error(err)

            if msg in response:
                # Success, move on
                break


# Main program ####################################################################
print("Flash Reader Started")

# Setup xmodem calls
modem = XMODEM(getc, putc)

# Parse arguments
parser = argparse.ArgumentParser()
parser.add_argument('port', help='Port Name', metavar = 'Port Name')
parser.add_argument('file', help = 'File Name', metavar = 'File Name')
args = parser.parse_args()

# Open serial port
try:
    port = serial.Serial(args.port, baudrate=115200, timeout=1)
except serial.serialutil.SerialException as msg:
    error(msg)

# Read File
print("read " + args.file + "\r")
port.write(("read " + args.file + "\r").encode())
readPort(b'Prepare for file', "Command Failed")

# Receive file via xmodem
print("Receiving file...")
stream = open(args.file, 'wb')
if not modem.recv(stream):
    error("File Transfer Failed")

print("")
print("File Transfer Complete!")

# Usage
# python flashReader.py comPort FileName
#
# Example
# python flashReader.py com14 presets.ini
