# Jexel-Driver
A PIC32-based programmable function generator and solenoid driver with 108 channels. 

This implementation employs a PIC32MZ2048ECH144 microcontroller to generate 108 independently programmable square waves. The computer interface is an FTDI FT232 chip, which becomes a USB-Serial interface and appears in the computer as a regular COM port. Sending strings to the COM port with the right settings will program/activate the "jexels" (short for jet-pixels), allowing interesting combinations. 

The current effective sampling rate is 49.8kS/s (measured with a scope given the delays of the program), remembering each channel is one-bit. There's a delay of 200ns (one cycle) between the update of each port in the uC. Since there's 10 ports being used, a total 1.8us delay exists between PORTA and PORTK. I deemed this delay insignificant since the maximum square wave drive frequency intended was 200Hz, thus rendering this maximum 1.8us delay less than 0.13 degrees error in the phase between the waves.

Let me know if you find this useful. Things that could be done to improve the application (which I decided not to):
-Increase data density in the serial port to allow a live data stream from the PC instead of a one-off program
-Make drivers with different capacities (more amps, higher voltage, etc) for different solenoid needs
-Isolate better the 24V line from the PIC (i.e., have a connector directly to the driver board).

I used MPLAB X IPE for the microcontroller program. EasyEDA (yes, I'm ashamed) for the electronic boards and JLCPCB for the printing service. The boards were designed to be hand-assembled (I used the hot plate method with solder paste - see here an example https://www.youtube.com/watch?v=2gkZX0r4cvQ). 
