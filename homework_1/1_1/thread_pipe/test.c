#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int fd[2];
int anamoni=0;

void *pipe_read(){
	char ch;
	int nread;
        while(1){
		while(anamoni==0){};
		nread = read(fd[0],&ch,1);
		if(nread == -1){
		        perror("fail read");
		        exit(3);
	        }
		printf("Read pipeRead char %c\n", ch);
		sleep(3);
	}
}

void *pipe_write(){
	int nwrite;
	char ch='A';

	while(1){
		nwrite=write(fd[1],&ch,1);
		if(nwrite!=1){
			perror("fail write");
			exit(2);
		}

		printf("pipe write %c\n", ch);
		if(ch=='Z'){
			ch = 'A'-1;
		}
		ch++;
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
