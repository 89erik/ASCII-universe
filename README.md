# Gravity simulator
A gravity simulator in two dimensions. The simulation is visualized by the terminal.

The file 'two_planets.sim' is an example of a fairly stable two-planet solar system. 
To use it, run the program as follows:

$./gravity -o -c 0 -h 55 -w 55 < two_planets.sim

* -o 		means you are adding custom objects (needed to use the input file)
* -c 0 		means you want object 0 in the center at all times
* -h 55 	print area height 55 (your screen resolution must be at least 1080 pixels in height and your terminal filling the screen before run)
* -w 55		print area width 55
* < two_planets.sim uses the file two_planets.sim as the input for objects. If absent, you will be prompted to input your objects your self.

You can also use the help flag to get an explanation for all options:

$./gravity --help
