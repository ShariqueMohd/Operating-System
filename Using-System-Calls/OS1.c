/*
    To Reverse the content of a file in new file in a new directory
*/

#include<stdio.h>
#include<stdlib.h>
#include<syscall.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>

#define BUFF 1
#define lli long long int

char S[BUFF];   // since BUFF=1, therefore it is just a char,
                // though by increasing BUFF value, you can also use it as array of char

int main(int argc,char *argv[]){
    int len,fo,fw,i,isDir,file_size;
    char Dir[]="OS_Assignment",error[50],ch;
    size_t size_read;
    lli tmp;

    if(argc!= 2){
        strcpy(error,"Improper input! Correct format: ./a.out <filename>\n");
        write(2,error,sizeof(error));
        exit(1);
    }

    fo = open(argv[1],O_RDONLY);

    if(fo == -1){
        strcpy(error,"No such file could be found :(\n");
        write(2,error,sizeof(error));
        exit(1);
    }

    struct stat sb;
    if(stat(argv[1],&sb)!=0){
        exit(EXIT_FAILURE);
    }
    file_size = (int) sb.st_size;

    isDir = mkdir(Dir,00711);

    if(isDir == -1){
        strcpy(error,"Sorry! Directory could not be created\n");
        write(2,error,sizeof(error));
        exit(1);
    }

    strcat(Dir,"/");
    strcat(Dir,argv[1]);

    fw = open(Dir, O_RDWR | O_CREAT, 00600);

    int cntlen=0,pos=0;
    while(1){
        //seek to end
        //printf("Entered the while(1) loop\n");

        cntlen+=BUFF;
        tmp = lseek(fo,file_size-1-cntlen,SEEK_SET);
        if(tmp == -1){
            //printf("lseek kaam nahi kar rha hai\n");
            break;
        }
        size_read = read(fo,S,BUFF);
        len = (int) size_read;
        if(len<=0 || size_read == -1 || size_read == 0){
            //printf("read karne me dikkat hai\n");
            break;
        }
        //printf("String: %s & len =  %d\n",S,len);

        for(i=0;i<(len>>1);i++){
            ch=S[i];
            S[i]=S[len-1-i];
            S[len-1-i]=ch;
        }
        //printf("String: %s & len =  %d\n",S,len);
        write(fw,S,size_read);

        /*
        tmp = lseek(fo,-(cntlen+BUFF),SEEK_END);
        size_read=read(fo,S,BUFF);
        len=(int)size_read;
        if(len<=0 || size_read == -1 || size_read == 0){
            printf("read karne me dikkat hai\n");
            break;
        }

        cntlen+=BUFF;

        // swapping
        ch = S[0];
        S[0]=S[1];
        S[1]=ch;*/


    }

    close(fw);
    close(fo);

    exit(0);

}
