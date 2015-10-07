#include <stdio.h>
#include <string.h>
#define SIZE 100

void pipe_init(int size);
void pipe_write(char c);
void pipr_close();
int pipe_read(char *c);
 
  //int in;
 // int out;
  
int main(int argc,char* argv[]){
  char str[SIZE];
 
  int array_size;
  int i;
  char fifo[SIZE];
  
  printf("enter size of array: \n");
  scanf ("%d",&array_size );
  
  pipe_init(array_size);
  
  printf("enter data: \n");
  scanf ("\t%s",str );
  
  for(i=0;str[i]!='\0';i++){
    pipe_write(str[i]);
  }
  
  return 0;
}
pipe_init(int size){
  int i;
  for(i=0;i<size;i++){
    //fifo[i]=0;    
  }
}
void pipe_write(char c){
  
}