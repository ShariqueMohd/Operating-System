#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/wait.h>

#define sz 1000	
int N;
key_t key=1234;
int shm_id;

void selectionSort(int *arr,int start,int last){
	int x,y,ind,tmp;
	for(x=start; x<=last; x++){
		ind = x;
		for(y=x+1; y<=last; y++){
			if(*(arr+ind)>*(arr+y)){
				ind = y;
			}
		}
		tmp = *(arr+x);
		*(arr+x) = *(arr+ind);
		*(arr+ind) = tmp;
	}
}

void mergesort(int start,int last){
	int i,j,k,temp,mid;
	int *arr;
	pid_t pid;
	
	arr = (int *)shmat(shm_id,NULL,0);
	if(arr == -1){
		perror("shmat");
		exit(1);
	}
	if(last-start+1<5){
		selectionSort(arr,start,last);
		return;
	}
	mid = (start+last)/2;
	pid = fork();
	if(pid<0){
		printf("Couldn't create a child process\n");
		exit(1);
	}
	if(pid==0){
		mergesort(start,mid);
		exit(0);
	}
	else{
		pid = fork();
		if(pid<0){
			printf("Couldn't create a child process\n");
			exit(1);
		}
		if(pid==0){
			mergesort(mid+1,last);
			exit(0);
		}
		while(waitpid(-1,NULL,0)){
			if(errno == ECHILD){
				break;
			}
		}
	}

	int tmp[100];
	i=start; j=mid+1; k=0;
	while(i<=mid && j<=last){
		if(arr[i]<arr[j]){
			tmp[k]=arr[i];
			k++; i++;
		}
		else{
			tmp[k]=arr[j];
			k++; j++;
		}
	}
	while(i<=mid){
		tmp[k]=arr[i];
		k++; i++;
	} 
	while(j<=last){
		tmp[k]=arr[j];
		k++; j++;
	}
	for(i=start;i<=last;i++){
		arr[i] = tmp[i];
	}
	return;
}

int main(){
	int i,j;
	int *arr;
	pid_t pid;
	
	printf("Enter N: ");
	scanf("%d",&N);
	shm_id = shmget(key,N*sizeof(int),IPC_CREAT|0666);
	if(shm_id<0){
		perror("shmget");
		exit(1);
	}

	arr = (int *)shmat(shm_id,NULL,0);
	if(arr == -1){
		perror("shmat");
		exit(1);
	}

	for(i=0;i<N;i++){
		scanf("%d",arr+i);
	}
	pid = fork();
	if(pid<0){
		printf("Couldn't create a child process\n");
		exit(1);
	}
	if(pid == 0){
		mergesort(0,N-1);
		exit(0);
	}
	else{
		while(waitpid(-1,NULL,0)){
			if(errno == ECHILD){
				for(i=0;i<N;i++){
					printf("%d ",arr[i]);
				}
				printf("\n");
				if(shmdt(arr)!=0){
					perror("shmdt");
				}
				shmctl(shm_id,IPC_RMID,NULL);
				break;
			}
		}
	}
	return;
}
