# -*- coding: utf-8 -*-
"""
Created on Tue Jul 15 20:31:08 2014

@author: stenkelfled
"""

import serial.tools.list_ports as list_ports
import serial

a = list_ports.main()
print a

ser = serial.Serial('COM8', 115200)
ser.write("Led")
ser.write("A0A2000201A1B0B400E1000000B1FF".decode('hex')) #switch color
ser.close()