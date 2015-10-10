#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

int fd[2];
int i_read, i_write, pipe_size;
int pipe_wait = 0;

void pipe_init(int size); //Init pipe prototype
void *pipe_read(); //Read pipe prototype
void *pipe_write(); //Write pipe prototype
void pipe_close();


int main(int argc, char *argv[]){
 	pthread_t pipeRead, pipeWrite;
	int size;

	printf("Give the size of pipe buf\n");
	scanf("%d", &size);
	while(size<=0){
		printf("Give number>0\n");
		scanf("%d",&size);
	}

        pipe_init(size);  

	pthread_create(&pipeWrite,NULL,pipe_write,NULL);
	pthread_create(&pipeRead,NULL,pipe_read,NULL);

	while(1){}
}

 void *pipe_read(){
	char *ch=NULL;
	int nread;
	ch = (char *)malloc(sizeof(char)*pipe_size);
	printf("Read pipe start\n");
        while(1){
		while(i_read==i_write){
			printf("Pipe buf is empty\n");
			while(pipe_wait==0){}
		}
		
		nread = read(fd[0],ch,sizeof(char)*pipe_size);
		//lseek(fd[0],0,SEEK_SET);
		if(nread == -1){
		        perror("Fail read");
		        exit(3);
	        }
		pipe_wait = 0;
		printf("Pipe read char %c \n", ch[i_read]);
		i_read = (i_read+1)%pipe_size;
	}
} 

 void *pipe_write(){
	int nwrite,i;
	char *ch=NULL;
	char x;
        ch = (char *)malloc(sizeof(char)*pipe_size);
	for(i=0;i<pipe_size;i++)ch[i]='\0';
        printf("Write buf strat\n");
	while(1){
		
		while(i_read == ( (i_write+1)%pipe_size )){
			printf("Pipe buf is full\n");
			pipe_wait = 1;
			while(pipe_wait==1){}
		}
		scanf(" %c", &x);
		ch[i_write]=x;
 		nwrite=write(fd[1],ch,sizeof(char)*pipe_size);
		//lseek(fd[1],0,SEEK_SET);
		if(nwrite==-1){
			perror("Fail write");
			exit(2);
		}
		pipe_wait=1;
                i_write= (i_write+1)%pipe_size;
	}
} 

void pipe_init(int size){

        printf("Initialize pipe\n");

        i_read = 0;
	i_write = 0;
	pipe_size = size;
        if (pipe(fd)<0){perror("Fail create pipe");exit(1);}
        if (fcntl(fd[0],F_SETPIPE_SZ, sizeof(char)*size)<0){perror("Set pipe size failed.");exit(2);}
        if (fcntl(fd[1],F_SETPIPE_SZ, sizeof(char)*size)<0){perror("Set pipe size failed.");exit(3);}
         
	
}
