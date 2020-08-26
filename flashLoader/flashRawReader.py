import argparse
import serial
import sys
import os
import time

# Requires PySerial - pip install pyserial

# Declare function calls
def error(msg):
    print ('\r\nError: %s' % msg)
    sys.exit(1)


# Main program ####################################################################
print("Flash Raw Reader Started")

# Parse arguments
parser = argparse.ArgumentParser()
parser.add_argument('port', help='Port Name', metavar = 'Port Name')
args = parser.parse_args()

# Open serial port
try:
    port = serial.Serial(args.port, baudrate=115200, timeout=1)
except serial.serialutil.SerialException as msg:
    error(msg)

# Read Flash
saveFile = open("rawFlash.txt","w") 
sector = 0
while sector < 0x10000:
    txData = "rawread " + hex(sector)[2:] + "\r"
    port.write(txData.encode())
    while 1:
        response = port.readline()
        response = response.decode()
        print(response)

        rxFlag = "raw "
        if sector < 0x1000:
            rxFlag += "0"
        if sector < 0x100:
            rxFlag += "0"
        if sector < 0x10:
            rxFlag += "0"
        rxFlag += hex(sector)[2:].upper()

        if rxFlag in response:
            saveFile.write(response)
            response = ""
            break
    
    sector += 1

saveFile.close()
print("")
print("Flash Reading Complete!")

# Usage
# python flashReader.py comPort
#
# Example
# python flashReader.py com14
