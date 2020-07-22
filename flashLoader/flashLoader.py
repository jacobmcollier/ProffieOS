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
print("Flash Loader Started")

# Setup xmodem calls
modem = XMODEM(getc, putc)

# Parse arguments
parser = argparse.ArgumentParser()
parser.add_argument('port', help='Port Name', metavar = 'Port Name')
parser.add_argument('directory', help = 'Directory Location', metavar = 'Directory Location')
args = parser.parse_args()

# Open serial port
try:
    port = serial.Serial(args.port, baudrate=115200, timeout=1)
except serial.serialutil.SerialException as msg:
    error(msg)

# Format Flash
print("format")
port.write("format\r".encode())
readPort(b'Filesystem Mounted', "Flash Formatting Failed")

# Get a list of all subdirectories to program to flash
print("Scanning directory: ", args.directory)

if not os.path.isdir(args.directory):
    error("Directory invalid!")

# Step through all the subdirectories
# - There shouldn't be any files at this level
for r,d,f in os.walk(args.directory):
    for dir in d:
        print("mkdir " + dir)
        port.write(("mkdir " + dir + "\r").encode())
        readPort(b'Created directory', "mkdir Failed")
        
        # Step through all the files in the subdirectory
        # - There shouldn't be any subdirectories at this level
        for R,D,F in os.walk(os.path.join(r, dir)):
            for file in F:
                print("write " + dir + "/" + file)
                port.write(("write " + dir + "/" + file + "\r").encode())
                readPort(b'Ready for file', "write Failed")

                # Send file via xmodem
                print("Transfering file...")
                stream = open(os.path.join(R, file), 'rb')
                if not modem.send(stream):
                    error("File Transfer Failed")
                
                readPort(b'Saved file', "xmodem Failed (Flash is likely full)")

print("")
print("Flash Loading Complete!")

# Usage
# python flashLoader.py comPort Path
#
# Example
# python flashLoader.py com14 C:\Users\jacob\Downloads\Sound_Fonts
