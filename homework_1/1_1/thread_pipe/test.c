#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_BUF 30
int fd[2];
int anamoni=0;

void *pipe_read(){
	char ch[MAX_BUF];
	int nread;
        while(1){
		memset(ch,0,sizeof(ch));
		while(anamoni==0){};
		nread = read(fd[0],ch,sizeof(ch));
		if(nread == -1){
		        perror("fail read");
		        exit(3);
	        }
		printf("Read pipeRead char %s\n", ch);
		sleep(3);
		anamoni=0;
	}
}

void *pipe_write(){
	int nwrite, i;
	char ch[MAX_BUF];
	
	while(1){

 		memset(ch, 0, sizeof(ch));
		scanf("%s", ch); 
 		nwrite=write(fd[1],ch,strlen(ch));
		if(nwrite==-1){
			perror("fail write");
			exit(2);
		}

		printf("pipe write %s\n", ch);
		anamoni=1;
		sleep(3);
	}
}

int main(int argc, char *argv[]){
 	pthread_t pipeRead, pipeWrite;
	
	if(pipe(fd)<0){perror("fail pipe"); exit(1);}	

	pthread_create(&pipeWrite,NULL,pipe_write,NULL);
	pthread_create(&pipeRead,NULL,pipe_read,NULL);

	while(1){}
}
