#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

char *buffer;
int i_read, i_write, pipe_size;
int pipe_wait = 0;
int p_close = 0;
char c_term;

void pipe_init(int size); //Init pipe prototype
void *readT(); //Read thread prototype
void *writeT(); //Write thread prototype
void pipe_write(char c); //write pipe prototype
int pipe_read(char *c); //read pipe prototype
void pipe_close();      // close prototype


int main(int argc, char *argv[]){
 	pthread_t pipeRead, pipeWrite;
	int size, pCread, pCwrite;
	char buf;

	printf("Give the size of pipe buf: ");
	scanf("%d", &size);
	while(size<=0){
		printf("Give number>0\n");
		scanf("%d",&size);
	}
	printf("Give terminate char: ");
	scanf(" %c", &c_term);
        while((buf=getchar())!='\n');
        pipe_init(size);  

<<<<<<< HEAD
	pCwrite=pthread_create(&pipeWrite,NULL,writeT,NULL);
	if(pCwrite){
		perror("Fail pthread_create write thread\n");
		exit(1);
	}
	pCread=pthread_create(&pipeRead,NULL,readT,NULL);
        if(pCread){
		perror("Fail pthread_create read thread\n");
		exit(1);
	}
=======
	pthread_create(&pipeWrite,NULL,writeT,NULL);
	pthread_create(&pipeRead,NULL,readT,NULL);

<<<<<<< HEAD
	while(1){}
=======
>>>>>>> 29ad93c74809351f5844ac3a141febec736ce946
	while(p_close!=1){}
>>>>>>> b9d8a11baf7f64b017083e0f98978f4006225a40
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
			if(readRes==0)break;
			pipe_wait=0;
			printf("Pipe read char: %c\n", x);
			i_read=(i_read+1)%pipe_size;
		}
	}
	pipe_wait = 2;
        return NULL;
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

		if(x==c_term){
			pipe_close();
			break;
		}
	}
	return NULL;
}

 int pipe_read(char *c){
	 *c=buffer[i_read];
	 if(buffer[i_read]==c_term){
		return 0;
	 }else{
		return 1;
	 }
	
} 

 void pipe_write(char c){
	 buffer[i_write]=c;
	 return;
} 

void pipe_close(){
	while(pipe_wait!=2){}
		free(buffer);
		p_close=1;

}
