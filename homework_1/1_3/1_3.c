#include <stdio.h>
#include <stdlib.h>

void print_array(int *num_array, int array_size, int key);//Print array prototype
void quicksort(int **, int, int);//quicksort prototype

int main(int argc, char *argv[]){

	int array_size, num_generate, i;
	int *num_array;

	array_size=-1;
	num_generate=-1;
	num_array=NULL;

	printf("Enter the size of array: ");
	scanf(" %d", &array_size);
        while(array_size<=0){
		printf("Give number greater than 0: ");
                scanf(" %d", &array_size);
	}

	printf("Enter 0 to generate randomly the numbers or 1 to enter manualy the numbers: ");
	scanf(" %d", &num_generate);
	while( (num_generate != 0) && (num_generate != 1) ){
		printf("Please give num 0 to generate randomly the numbers or 1 to enter manualy the numbers: ");
		scanf(" %d", &num_generate);
	}
	
	num_array=(int*) malloc(array_size*sizeof(int));
	if (num_array==NULL){
		perror("Fail to allocate Number array");
		exit(1);
	}

	if(num_generate == 0){
		for(i=0;i<array_size;i++)num_array[i]=rand()%1000;
	}else{
		for(i=0;i<array_size;i++){
			printf("Enter a number[%d]: ", i);
			scanf(" %d", &num_array[i]);
		}
	}
        print_array(num_array, array_size, 0);


	quicksort(&num_array, 0, array_size-1);
	
        print_array(num_array, array_size, 1);
          
	return 0;
}	                                                                                                                                                                                                 
void print_array(int *array, int array_size, int key){

	int i;

	if (key == 0){
        	printf("Unsorted array: ");
		for(i=0;i<array_size;i++)printf("%d ", array[i]);	
        	printf("\n"); 
	}else{
 		printf("Sorted array: ");
		for(i=0;i<array_size;i++)printf("%d ", array[i]);	
        	printf("\n"); 
	}

}

void quicksort(int **num_array, int first, int last){
	int pivot, j, temp, i;
	int *array=NULL;
	array=*num_array;
	
	pivot=first;
	i=first;
	j=last;
		
	if(first<last){
		while(i<j){
			while((array[i]<=array[pivot]) && (i<last))i++;
			while(array[j]>array[pivot])j--;

			if(i<j){
				temp=array[i];
				array[i]=array[j];
				array[j]=temp;
			}
		}
	}

	temp = array[pivot];
	array[pivot]=array[j];
	array[j]=temp;

       
	quicksort(&array, first, j-1);
	quicksort(&array, j+1, last);
	
}
