# MUX_servos3
Multiplexer for servo signals with Attiny/Arduino

For a VTOL model with a stabilization which is too strong in horizontal flight I wanted to switch servo signals in flight.
A control signal will choose the signals to be output. I use a 4053 multiplexer chip. There may be better chips, but these I had available and it doesn't matter for the software.
I use an ATTiny45 to get a small PCB.

The input signal has 5 pulse width areas:

1000-1199 1200-1399 1400-1600  1800-1601  2000-1801ms

   SS        MS         MM        SM         SS        
M stands for Master input, S for Slave input
MM is also used if no valid control input is found.

I have two output control signals, so I can switch Aileron and Elevator independently.
In flight I will start hovering with MM and the servos controlled by the stabilization unit. Then I switch to horizontal flight. After the transition I can switch from stabilization controlled servos to direct TX controlled servos. The 4053 has 3 2-to-1 multiplexers.
