#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

typedef struct qs_pkg{
	int *num_array;
	int left;
	int right;
} qsPkg;

void print_array(int *, int, int); 	//print array prototype
void *quicksort(void *);		//quicksort prototype
void swap(int [], int, int);

int main(int argc, char *argv[]){

    qsPkg qs;

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

	while( (num_generate!=0) && (num_generate!=1) ){
		printf("Please give num 0 to generate randomly the numbers or 1 to enter manualy the numbers: ");
		scanf(" %d", &num_generate);
    }

    	num_array=(int*) malloc(array_size*sizeof(int));

	if (num_array==NULL){
        	perror("Fail to allocate Number array");
        	exit(1);
    }

    	if(num_generate==0){
        	for(i=0;i<array_size;i++)num_array[i]=rand()%1000;
    	}else{
			srand(time(NULL));
        	for(i=0;i<array_size;i++){
            		printf("Enter a number[%d]: ", i);
            		scanf(" %d", &num_array[i]);
        	}
    	}

	print_array(num_array, array_size, 0);

	qs.num_array=num_array;
	qs.left=0;
	qs.right=array_size-1;

	quicksort(&qs);

	print_array(num_array, array_size, 1);

	free(qs.num_array);
    return 0;
}


void print_array(int *array, int array_size, int key){

	int i;

    	if (key==0){
        	printf("Unsorted array: ");
        	for(i=0;i<array_size;i++)printf("%d ", array[i]);
        	printf("\n");
    	}else{
        	printf("Sorted array: ");
        	for(i=0;i<array_size;i++)printf("%d ", array[i]);
        	printf("\n");
    	}


}


void *quicksort(void *qs){

	int *num_array;
	int left, right, pivot, i, last_swap_pos, thread_status;
	unsigned int seed=1;

    pthread_t left_thread, right_thread;

	qsPkg *pkg;
	qsPkg lpkg;
	qsPkg rpkg;

	pkg=qs;

	num_array=pkg->num_array;
	left=pkg->left;
	right=pkg->right;

	if(left>=right)return NULL;

	pivot=(rand_r(&seed) % (right-left+1))+left;
	swap(num_array, left, pivot);
        last_swap_pos=left;

	for(i=left+1;i<=right;i++) {
		if(num_array[i]<num_array[left]){
			  swap(num_array, ++last_swap_pos, i);
		}
	}
		swap(num_array, left, last_swap_pos);

       lpkg.num_array=num_array;
       lpkg.left=left;
       lpkg.right=last_swap_pos-1;

       rpkg.num_array=num_array;
       rpkg.left=last_swap_pos+1;
       rpkg.right=right;

       thread_status=pthread_create(&left_thread, NULL, quicksort, &lpkg);
	   if(thread_status){
		   perror("Fail to create left pkg\n");
		   exit(1);
	   }
       thread_status=pthread_create(&right_thread, NULL, quicksort, &rpkg);
	   if(thread_status){
		   perror("Fail to create left pkg\n");
		   exit(1);
	   }
       sleep(1);

     return NULL;
}

void swap(int num_array[], int i, int j){
	int temp;

	temp=num_array[i];
	num_array[i]=num_array[j];
	num_array[j]=temp;

}
