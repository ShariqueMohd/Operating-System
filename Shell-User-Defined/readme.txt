BY: MOHD SHARIQUE 

I have made a main program named shell.c which is the driver program. I have also made two separate header files-> 1) I\O.h 2) Process.h
	I\O.h deals with the error,input and output part of the program. The functions of this header file are being used by functions in shell.c like echo(), presentWorkingDir() etc.
	Process.h contains deals with implementing foreground and background process for the shell. Both the methods call a function returnList() in the same header file, that tokenizes input command, while the function controllerSIGCHLD() which checks and prints when the background process terminates. 

Instructions to run the program:
1) Move to the folder containing the assignment in terminal.
2) Type the following command one after the other:
> gcc shell.c -o SHELL
> SHELL


