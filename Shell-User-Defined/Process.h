/*
    This header file contains four different function:
    1) Function to return list of tokenized strings
    2) Controller for SIGCHLD
    3) Function for Front Processes
    4) Function for Background Processes
*/

#ifndef PROCESS_H
#define PROCESS_H

#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>


// used from the file calling/using this header file
extern char *HOME ;

// function that returns the tokenized sub-parts of 
// the whole(complete) command enteres by the user.
char** returnList(char *command){
	char **List = (char**)malloc(sizeof(char *)*20);
	int len = 0;
	char *part = strtok(command, " ");
	do{
		List[len] = part;	len += 1;
		part = strtok(NULL, " ");
	}
	while(part != NULL && part[0] != 0);

	List[len] = NULL;
	return List;
}

// function that checks for signal/interrupt,
// used to check for the process to end.
void controllerSIGCHLD(int sigNumber){

	pid_t pid;
	int status;
	while((pid = waitpid(-1,&status, WNOHANG|WUNTRACED)) != -1){
		char outputFile[100],message[100];
		
		sprintf(outputFile, "%s/OUTPUT/%d_output",HOME,pid);
		if(status == 0){
			int output = open(outputFile, O_RDONLY);
			char ch;
			while(read(output, &ch, 1) > 0 ){
				write(STDOUT_FILENO, &ch, 1);
			}
		}

		unlink(outputFile);
		sprintf(message,"%d has ended with status %d\n",pid,status);
		outputShell(message);
	}
}

int total(char command[1000], char *commands[100]){
	int total = 0;

	char *token = strtok(command, "|");
	while(token != NULL){
		commands[total] = (char*) malloc(100*sizeof(char));
		strcpy(commands[total], token);
		total++;

		token = strtok(NULL, "|");
	}
	return total;
}

void spawnCommand(int in, int out, char *argv[1000]){
	int forkRet = fork();

	if(forkRet == -1){
		char *err = strerror(errno);
		outputShell(strcat(err, "\n"));
	}
	else if(forkRet == 0){
		dup2(in, STDIN_FILENO);
		dup2(out, STDOUT_FILENO);

		int exeRet = execvp(argv[0], argv);
		if(exeRet == -1){
			char *err = strerror(errno);
			printf("%s\n", err);
			close(in);
			close(out);

			_Exit(0);
		}
	}
	else{
		while(waitpid(-1, NULL, 0)){
			if(errno == ECHILD)
				break;
		}
	}
	return;
}

int splitParameters(char *command, char *argv[1000], char *Redirn[5]){
	char *token = strtok(command, " ");
	int j=0;
	int isRedir = -1;
	while(token != NULL){
		if(strcmp(token, "<")==0){
			token = strtok(NULL, " ");
			Redirn[0] = (char*) malloc(100*sizeof(char));
			strcpy(Redirn[0], token);

			isRedir++;
		}
		else if(strcmp(token, ">")==0){
			token = strtok(NULL, " ");
			Redirn[1] = (char*) malloc(100*sizeof(char));
			strcpy(Redirn[1], token);

			isRedir += 2;
		}
		else if(strcmp(token, ">>")==0){
			token = strtok(NULL, " ");
			Redirn[1] = (char*) malloc(100*sizeof(char));
			strcpy(Redirn[1], token);

			isRedir += 4;
		}
		else if(strcmp(token, "&")!=0){
			argv[j] = (char*) malloc(100*sizeof(char));
			strcpy(argv[j], token);
			j++;
		}
		token = strtok(NULL, " ");
	}
	argv[j] = NULL;

	return isRedir;
}

void setFileDescriptors(int *in, int *out, int isRedir, char *Redirn[5]){
	if(isRedir == 0)
		*in = open(Redirn[0], O_RDWR | O_TRUNC, S_IRWXU);
	else if(isRedir == 1)
		*out = open(Redirn[1], O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
	else if(isRedir == 3)
		*out = open(Redirn[1], O_RDWR | O_APPEND | O_CREAT, S_IRWXU);
	else if(isRedir == 2 || isRedir == 4)
	{
		*in = open(Redirn[0], O_RDWR | O_TRUNC, S_IRWXU);
		*out = open(Redirn[1], O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
	}
}


// function for process running in-front(foreground process)
void frontProcess(char *inp){
	char *subCommands[100];
	char *argv[1000];
	char *Redirn[5];
	int totalDistinct = total(inp, subCommands);

	int pipeFd[2];
	int in = STDIN_FILENO;
	int out = STDOUT_FILENO;
	int i;

	pipe(pipeFd);

	for(i=0;i<=totalDistinct-2;i++)
	{

		char *command = subCommands[i];
		char *argvv[1000];
		char *innerRedirn[5];

		out = pipeFd[1];

		int isRedir = splitParameters(command, argvv, innerRedirn);
		setFileDescriptors(&in, &out, isRedir, innerRedirn);

		if(in == -1 || out == -1)
		{
			printf("No such file exists\n");
			outputShell("");

			return;
		}

		spawnCommand(in, out, argvv);
		close(pipeFd[1]);

		in = pipeFd[0];
	}

	out = STDOUT_FILENO;

	int isRedir = splitParameters(subCommands[totalDistinct-1], argv, Redirn);
	setFileDescriptors(&in, &out, isRedir, Redirn);

	if(in == -1 || out == -1)
	{
		printf("No such file exists\n");
		outputShell("");

		return;
	}

	spawnCommand(in, out, argv);

	if(out != STDOUT_FILENO)
		close(out);
	if(in != STDIN_FILENO)
		close(in);

	close(pipeFd[0]);
	outputShell("");
}

/*
void frontProcess(char *command){

	char **L = returnList(command);
	char *cmd = L[0];

	int pid = fork();
	if(pid == 0) {
		execvp(cmd,L);

		outputShell("Invalid command: Couldn't find any such command\n");
		exit(1);
	}
	else {
		int status;
		waitpid(pid, &status, 0);
	}
}*/

// function for process running in background(background processes)
void backgroundProcess(char *command){

	int Cid = fork();

	if(Cid == -1){
		errorShell(errno);
	}
	else if(Cid == 0){
		int i = open("cInp", O_WRONLY | O_CREAT, S_IRWXU);
		int o = open("cOut", O_WRONLY | O_CREAT, S_IRWXU);

		dup2(i, 0);
		dup2(o, 1);

		frontProcess(command);

		_Exit(0);
	}
	else{
		char msg[1000] = "Process ID : ";
		char pId[1000];

		sprintf(pId, "%d", Cid);
		strcat(msg, pId);
		strcat(msg,"\n");

		outputShell(msg);
	}

}

#endif
