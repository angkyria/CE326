#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_BUF 10
int fd[2];
int i_read, i_write;


void *pipe_read(){
	char ch[MAX_BUF];
	int nread;
        while(1){
		sleep(3);
		while(i_read==i_write){printf("Pipe buf is empty\n");sleep(5);}
		nread = read(fd[0],ch,sizeof(char)*MAX_BUF);
		//lseek(fd[0],0,SEEK_SET);
		if(nread == -1){
		        perror("fail read");
		        exit(3);
	        }
		printf("Read pipeRead char %c gramena sto solina  %s\n", ch[i_read], ch);
		i_read = (i_read+1)%MAX_BUF;
	}
}

void *pipe_write(){
	int nwrite,i;
	char ch[MAX_BUF];
	char x;

	for(i=0;i<MAX_BUF;i++)ch[i]='\0';

	while(1){
		
		while(i_read == ( (i_write+1)%MAX_BUF )){printf("Pipe buf is full\n");sleep(3);}
		scanf(" %c", &x);
		ch[i_write]=x;
 		nwrite=write(fd[1],ch,sizeof(char)*MAX_BUF);
		//lseek(fd[1],0,SEEK_SET);
		if(nwrite==-1){
			perror("fail write");
			exit(2);
		}
                i_write= (i_write+1)%MAX_BUF;
		printf("pipe write %s\n", ch);
		//sleep(1);
	}
}

int main(int argc, char *argv[]){
 	pthread_t pipeRead, pipeWrite;
 	i_read = 0;
	i_write = 0;           
	if(pipe(fd)<0){perror("fail pipe"); exit(1);}	

	pthread_create(&pipeWrite,NULL,pipe_write,NULL);
	pthread_create(&pipeRead,NULL,pipe_read,NULL);

	while(1){}
}
