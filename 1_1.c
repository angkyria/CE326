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
  int *fifo_root=NULL;
  
  printf("enter size of array: \n");
  scanf ("%d",&array_size );
  
  fifo_root=pipe_init(array_size);
  printf("%d",fifo_root);

  return 0;
}
pipe_init(int size){
  char *array;
  array=(char *)malloc(size*sizeof(char));
  return *array;

}

void pipe_write(char c){
  
}