/*
    To Check :
    1) That the Directory has been created or not
    2) The directory permission for the same
    3) That the file has been created or not
    4) The output file permissions
    5) Whether the file has been reversed or not
*/

#include<stdio.h>
#include<stdlib.h>
#include<syscall.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/stat.h>

#define BUFF 1
#define lli long long int

char S[BUFF],ONE[BUFF],TWO[BUFF];
char check[5][100],ans[5][50],perm[10];
int fc,file_size,fo,fw;
size_t size_read;

void init(){
    strcpy(check[0],"Checking whether Assignment Directory has been created : ");
    strcpy(check[1],"Checking the permission for the Assignment Directory permissions : ");
    strcpy(check[2],"Checking whether the Output file has been created : ");
    strcpy(check[3],"Checking the permission for the Output file : ");
    strcpy(check[4],"Checking if the file has been reversed : ");
}

void checkPermissions(char Name[100]){
    struct stat sb;
    stat(Name,&sb);

    strcpy(perm,"");

    //file OR directory
	if((S_ISDIR(sb.st_mode))){
        strcat(perm,"d");
	}
	else{
        strcat(perm,"-");
	}

	// #1
    if((sb.st_mode & S_IRUSR)){
        strcat(perm,"r");
	}
	else{
        strcat(perm,"-");
	}

	// #2
	if((sb.st_mode & S_IWUSR)){
        strcat(perm,"w");
	}
	else{
        strcat(perm,"-");
	}

    // #3
    if((sb.st_mode & S_IXUSR)){
        strcat(perm,"x");
	}
	else{
        strcat(perm,"-");
	}

	// #4
    if((sb.st_mode & S_IRGRP)){
        strcat(perm,"r");
	}
	else{
        strcat(perm,"-");
	}

	// #5
	if((sb.st_mode & S_IWGRP)){
        strcat(perm,"w");
	}
	else{
        strcat(perm,"-");
	}

	/* #6
	if((sb.st_mode & S_IWGRP)){
        strcat(perm,"w");
	}
	else{
        strcat(perm,"-");
	}*/

	// #6
	if((sb.st_mode & S_IXGRP)){
        strcat(perm,"x");
	}
	else{
        strcat(perm,"-");
	}

	// #7
	if((sb.st_mode & S_IROTH)){
        strcat(perm,"r");
	}
	else{
        strcat(perm,"-");
	}

	// #8
	if((sb.st_mode & S_IWOTH)){
        strcat(perm,"w");
	}
	else{
        strcat(perm,"-");
	}

	// #9
	if((sb.st_mode & S_IXOTH)){
        strcat(perm,"x");
	}
	else{
        strcat(perm,"-");
	}

	strcat(perm,"\n");
}

void createnew(char name[]){
    struct stat sb;
    if(stat(name,&sb)!=0){
        exit(EXIT_FAILURE);
    }
    file_size = (int) sb.st_size;
    fc = open("revREV.txt", O_RDWR | O_CREAT, 0700);

    int cntlen=0,pos=0,tmp,len,i;
    char ch;
    while(1){
        //printf("Entered the while(1) loop\n");

        cntlen+=BUFF;
        tmp = lseek(fw,file_size-cntlen,SEEK_SET);
        if(tmp == -1){
            //printf("lseek kaam nahi kar rha hai\n");
            break;
        }
        size_read = read(fw,S,BUFF);
        len = (int) size_read;
        if(len<=0 || size_read == -1 || size_read == 0){
            // printf("read karne me dikkat hai\n");
            break;
        }
        //printf("String: %s & len =  %d\n",S,len);
        //printf("Length:\t%d S[0]=%c\n",len,S[0]);
        for(i=0;i<(len>>1);i++){
            ch=S[i];
            S[i]=S[len-1-i];
            S[len-1-i]=ch;
        }
        //printf("String: %s & len =  %d\n",S,len);
        write(fc,S,size_read);
    }

}

int main(int argc,char *argv[]){

    int len,i,isDir=0,file_size,status=-1,flag=1,success;
    char Dir[]="OS_Assignment",error[50],ch;   // doubt over the name of Dir
    lli tmp;

    if(argc != 3){
        strcpy(error,"Improper input! Correct format: ./a.out <OriginalFilename> <FinalOutputFile>\n");
        write(2,error,sizeof(error));
        exit(1);
    }

    init();

    fo = open(argv[1], O_RDWR);
    if(fo == -1){
        strcpy(error,"Original file could be found :(\n");
        write(2,error,sizeof(error));
        exit(1);
    }

    // Number #1:
    struct stat sb;
    if(stat(Dir,&sb) == 0 && S_ISDIR(sb.st_mode)){
        isDir=1;
    }
    if(isDir == 0){
        flag=0;
        strcpy(ans[0],"No\n");
    }
    else{
        strcpy(ans[0],"Yes\n");
        status+=1;
    }

    // Number #2:
    if(flag){
        checkPermissions(Dir);
        strcpy(ans[1],perm);
        status+=1;
    }

    // Number #3:
    if(flag){
        strcat(Dir,"/");
        success = chdir(Dir);
        if(success == -1){
            flag=0;
            strcpy(ans[2],"No\n");
        }
    }

    if(flag){
        fw = open(argv[2],O_RDWR);
        if(fw==-1){
            flag=0;
            strcpy(ans[2],"No\n");
        }
        else{
            status+=1;
            strcpy(ans[2],"Yes\n");
        }
    }

    // Number #4:
    if(flag){
        checkPermissions(argv[2]);
        strcpy(ans[3],perm);
        //write(2,perm,sizeof(perm));
        status+=1;
    }

    // Number #5:
    int similar=1;
    if(flag){
        createnew(argv[2]); //copied content to new file that is reversed of reversed

        lseek(fo,0,SEEK_SET);
        lseek(fc,0,SEEK_SET);

        int cntlen=0,pos=0;
        while(1){
            //seek to end
           //printf("Entered the while(1) loop\n");

           size_read = read(fo,ONE,BUFF);
           if(size_read == 0 || size_read == -1){
                break;
           }
           size_read = read(fc,TWO,size_read);

           for(i=0;i<(int)size_read;i++){
                if(ONE[i] != TWO[i]){
                    similar = 0;
                    flag = 0;
                    break;
                }
           }
        }
    }

    if(similar == 0 || flag == 0){
        strcpy(ans[4],"No\n");
    }
    else{
        status += 1;
        strcpy(ans[4],"Yes\n");
    }

    for(i=0;i<=status;i++){
        write(2,check[i],sizeof(check[i]));
        write(2,ans[i],sizeof(ans[i]));
    }

    if(status<4){
        for(i=status+1;i<5;i++){
            write(2,check[i],sizeof(check[i]));
            write(2,"No\n",sizeof("No\n"));
        }
    }

    return 0;
}
