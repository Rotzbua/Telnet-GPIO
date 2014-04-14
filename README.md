Telnet-GPIO
===========

This is the Code for a Telnet GPIO-Server based on an Arduino Mega 2560 and the fitting Ethernet-shield.
Please copy the code from the file into your new Arduino Sketch.


Here is a list for the Syntax and the commands you have to send to the telnet-GPIO-server:

help        lists information about the program
kill        sets all outputs to high-state
get-all     lists the ID and the current state of all inputs
X.Y         Switch an output on and off. X ist the Number of the Output (01-10) 
            and Y is the state (1 for low and o for High)
A.X         read analog Value from Pin X (will return value from 0-1023)


