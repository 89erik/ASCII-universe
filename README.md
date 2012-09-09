Gravity simulator. This version uses the terminal to display the simulation.
I plan to write a GUI for it eventually. 

The file 'input' is an example of a fairly stable two-planet solar system. 
To use it, run the program as follows:

$./gravity -o -c 0 -h 55 -w 55 < input

-o 	means you are adding custom objects (from the input file)

-c 0 	means you want object 0 in the center at all times

-h 55 	print area heigh 55 (your screen resolution must be at least 1080 pixels
	in height and your terminal filling the screen before run)

-w 55	print area width 55

< input	means you are using the file input as input. If absent, you can input
	your objects your self

Also you can use the help flag to get an explanation for all options:
$./gravity --help
