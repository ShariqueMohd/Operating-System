/*
    NAME: MOHD SHARIQUE
    ROLL NO: 20161081
*/
#ifndef IO_H
#define IO_H

#include<unistd.h>
#include<string.h>

int outputShell(char *);
int errorShell(int);
char* inputShell();

// function to print output on the terminal
int outputShell(char *output){
	if(output == 0)
		return 0;
	char *o = (char *)malloc(strlen(output)+2);
	strcpy(o,output);
	return write(STDOUT_FILENO,o,strlen(o));
}

int errorShell(int err){
	char *e = strerror(err);
	char *error = (char *)malloc(strlen(e)+2);
	strcat(error,"\n");
	return write(STDERR_FILENO,error,strlen(error));
}

// function to take input
char* inputShell(){
	char inp;
	char *input = (char *)malloc(201);
	int len = 0,sz;		input[len] = '\0';	
	while(read(STDIN_FILENO,&inp,1)>0){
		if(inp == '\n'){
			break;
		}
		else{
			input[len] = inp;	len += 1;
			input[len] = '\0';
		}
	}

	while(*input != 0 && *input == ' ')
		++input;
	sz = strlen(input);
	while(sz>0 && input[sz-1] == ' ')
		input[sz--] = 0;
	

	return input;
}

#endif