/*
    NAME: MOHD SHARIQUE
    ROLL NO: 20161081

    -> To create a user defined shell in 'C'.
*/

#include<stdio.h>
#include<unistd.h>		 //getpid(), getcwd()
#include<sys/types.h>	 //type definitions, eg., pid_t
#include<sys/wait.h>	 //wait()
#include<sys/utsname.h> //for utsname 
#include<signal.h>		 //signal name constants and kill()
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include "I\O.h"
#include "Process.h"


// Contains path for home directory
char *HOME ;
// contains path information for present directory
char *PATH ;
int flagPipe = 0,flagRedirO=0,flagRedirI=0;
/* 
	systemName stores the name of the system,
	while the displayName stores something like this: @shariq-Vostro-3446:~> 
*/
char *systemName,*displayName;


// getPath gives the path for the pdirectory(working)
void getPath(){

	getcwd(PATH,201);
	if(strncmp(HOME,PATH,strlen(HOME)) == 0){
		int j = strlen(HOME),i=1;
		PATH[0] = '~';
		
		do{
			PATH[i] = PATH[j];
			++i; ++j;
		} while(PATH[j-1] != '\0');
	}
}

// function to change the directory
void changeDir(){
	int flag;
	char *dir = strtok(NULL," ");
	if(dir != NULL && strlen(dir)>0){
		flag = chdir(dir);
	}
	else{
		flag = chdir(HOME);
	}

	if(flag == -1){
		int errorNO = errno;
		errorShell(errorNO);
	}
	getPath();
}

// function to print present working directory
void presentWorkingDir(){
	char tmpPath[201];
	getcwd(tmpPath,201);
	strcat(tmpPath,"\n");
	outputShell(tmpPath);
}

// function for echo.
void echo(){
	char *str = strtok(NULL," ");
	if(str == NULL){
		str="";
	} 
	strcat(str,"\n");
	outputShell(str);
}

int main(){
	
	HOME =  (char*)malloc(200);
	PATH = (char*)malloc(200);
	char *command;
	char *token,*storedCommand,*cpy,*checkpipe,*cpy2;	
	getcwd(HOME,200);
	PATH = (char *)malloc(201);
	systemName = (char *)malloc(201);
	displayName = (char *)malloc(201);
	cpy2 = (char *)malloc(201);

	
	getPath();	//store the path in PATH

	struct utsname buff;
	uname(&buff);	// for name & information about current kernel

	signal(SIGCHLD, controllerSIGCHLD);		

	sprintf(systemName, "@%s",buff.nodename);
	while(1) {

		sprintf(displayName, "%s:%s> ", systemName, PATH);
		outputShell(displayName);
		//scanf("%[^\n]s",command);
		command = inputShell();
		if(strlen(command) == 0)
			continue;

		//printf("empty string par chal raha hai!\n");
		
		cpy = (char *)malloc(strlen(command)+1);
		strcpy(cpy,command);

		//printf("%s",cpy);
		token = strtok(command," ");
		storedCommand = token;

		if(strcmp(storedCommand,"exit") == 0){
			//printf("entered the if for exit");
			return 0;
		}
		else if(strcmp(storedCommand,"echo") == 0){
			//printf("checking\n");
			echo();
		}
		else if(strcmp(storedCommand,"pwd") == 0){
			//printf("Nahi chal rha hai :(\n");
			presentWorkingDir();
		}	
		else if(strcmp(storedCommand,"cd") == 0){
			changeDir();
		}
		else{
			
			// for new process 
			int clen = strlen(cpy);
			if(cpy[clen-1] != '&'){
				frontProcess(cpy);			 
			}
			else{
				cpy[strlen(cpy)-1] = 0;
				backgroundProcess(cpy);			//for background process
			}
			/*
			else{
				pid_t childPID = fork();
				if(childPID < 0){
					fprintf(stderr, "Error! Can't fork\n");
					return  -1;			
				}
				if(childPID == 0){
					
				}

			}*/
		}
	}

	return 0;
}