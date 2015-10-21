#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

char *buffer;
int i_read, i_write, pipe_size;
int pipe_wait = 0;

void pipe_init(int size); //Init pipe prototype
void *readT(); //Read thread prototype
void *writeT(); //Write thread prototype
void pipe_write(char c); //write pipe prototype
int pipe_read(char *c); //read pipe prototype
void pipe_close();      // close prototype


int main(int argc, char *argv[]){
 	pthread_t pipeRead, pipeWrite;
	int size;

	printf("Give the size of pipe buf: ");
	scanf("%d", &size);
	while(size<=0){
		printf("Give number>0\n");
		scanf("%d",&size);
	}

        pipe_init(size);  

	pthread_create(&pipeWrite,NULL,writeT,NULL);
	pthread_create(&pipeRead,NULL,readT,NULL);

	while(1){}
	return 0;
}
 
void pipe_init(int size){
        
	int i;
	char x='\0';
        printf("Initialize pipe\n");
        i_read = 0;
	i_write = 0;
	pipe_size = size;
       	buffer=(char *)malloc(sizeof(char)*pipe_size);
       	for(i=0;i<pipe_size;i++)buffer[i]=x;	
	
}   

void *readT(){

    	int readRes =1;
	char x='\0';

	while(readRes){
		if(pipe_size==1){
			if(pipe_wait==0){
				printf("Pipe buf is empty\n");
				while(pipe_wait==0){}
			}
			readRes=pipe_read(&x);
			pipe_wait=0;
			printf("Pipe read char: %c\n", x);
		}else{
			while(i_read==i_write){
				printf("Pipe buf is empty\n");
				pipe_wait=0;
				while(pipe_wait==0){}
			}
			readRes=pipe_read(&x);
			pipe_wait=0;
			printf("Pipe read char: %c\n", x);
			i_read=(i_read+1)%pipe_size;
		}
	}
}

void *writeT(){

	char x;
	while(1){
		if(pipe_size==1){
			if(pipe_wait==1){
				printf("Pipe buf is full\n");
				while(pipe_wait==1){}
			}
			scanf(" %c", &x);
			pipe_write(x);
			pipe_wait=1;
		}else{
			while(i_read == ((i_write+1)%pipe_size)){
				printf("Pipe buf is full\n");
				pipe_wait = 1;
				while(pipe_wait==1){}
			}
			scanf(" %c", &x);
			pipe_write(x);
			pipe_wait=1;
			i_write = (i_write+1)%pipe_size;
		}
	}
}

 int pipe_read(char *c){
	 *c=buffer[i_read];
	 return 1;
	
} 

 void pipe_write(char c){
	 buffer[i_write]=c;
	 return;
} 

void pipe_close(){

}
