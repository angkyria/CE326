#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#define RIGHT_PASS 10
 
void *left_car( void *i );
void *right_car( void *i );
void *monitor();
void car_cross( char direction );
void car_leave( char direction );
 
pthread_mutex_t end, mutex;
pthread_cond_t left, right, monitor_c;

char *car_tail, curr_dir;
int size_of_tail, size_of_left_tail, size_of_rigth_tail, bridge_capacity;



int main(int argc, char *argv[]){

	pthread_t monitor_t, *left_t, *right_t;
    char c;
    int i, thread_status,mtx_status, cond_status;

    printf("Give the cars directions. L or l for the left and R or r for the right direction: ");
    i=0; size_of_left_tail=0; size_of_rigth_tail=0;
    do{

        c=getchar();
        if( ( c == 'r') || ( c == 'R') ){

            car_tail = (char *) realloc( car_tail, (i+1) * sizeof(char));
            car_tail[i] = 'r';
            size_of_rigth_tail++;
            i++;

        }

        if( ( c == 'l') || ( c == 'L') ){

            car_tail = (char *) realloc( car_tail, (i+1) * sizeof(char));
            if(car_tail == NULL){
                free(car_tail);
                perror("Error reallocating memory");
                exit(3);
            }
            car_tail[i] = 'l';
            size_of_left_tail++;
            i++;

        }

    }while(c!='\n');
    size_of_tail=i;
    curr_dir=car_tail[0];

    printf("Give the maximum number of cars over the bridge: ");
    scanf(" %d", &bridge_capacity);

    mtx_status=pthread_mutex_init(&end, NULL);
    if(mtx_status){
	    perror("Fail init mutex end\n");
	    exit(1);
    }               
    mtx_status=pthread_mutex_init(&mutex, NULL);
    if(mtx_status){
	    perror("Fail init mutex end\n");
	    exit(1);
    }               
    cond_status=pthread_cond_init(&left, NULL);
    if(cond_status){
	    perror("Fail init cond left\n");
	    exit(1);
    }                
    cond_status=pthread_cond_init(&right, NULL);
    if(cond_status){
	    perror("Fail init cond right\n");
	    exit(1);
    }
                                     
    cond_status=pthread_cond_init(&monitor_c, NULL);
    if(cond_status){
	    perror("Fail init cond monitor\n");
	    exit(1);
    }                                



    if(size_of_left_tail!=0){
	    left_t=(pthread_t *) malloc(size_of_left_tail*sizeof(pthread_t));
	    if(left_t==NULL){

		    perror("Fail allocate left direction cars\n");
		    exit(2);
	    }
	    for(i=0;i<size_of_left_tail;i++){
             
		    thread_status=pthread_create(&left_t[i], NULL, left_car, (void *)(intptr_t)i);
		    if(thread_status){

			    perror("Fail create left car\n");
			    exit(2);
		    }
	    }

    }else{
	    printf("No car on the left direction\n");
    }

    if(size_of_rigth_tail!=0){
             right_t=(pthread_t *) malloc(size_of_rigth_tail*sizeof(pthread_t));
	    if(right_t==NULL){

		    perror("Fail allocate right direction cars\n");
		    exit(2);
	    }
	    for(i=0;i<size_of_left_tail;i++){
             
		    thread_status=pthread_create(&right_t[i], NULL, right_car, (void *)(intptr_t)i);
		    if(thread_status){

			    perror("Fail create right car\n");
			    exit(2);
		    }
	    }
    }else{
	    printf("No car on the right direction\n");
    }

    

    thread_status=pthread_create(&monitor_t,NULL, monitor, NULL);
    if(thread_status){
	    perror("Fail create monitor threat\n");
	    exit(2);
    }

    pthread_mutex_lock(&end);
    

                            sleep(5);
    printf("Succefull pass\n");
    pthread_cond_destroy(&monitor_c);
    pthread_cond_destroy(&left);
    pthread_cond_destroy(&right);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&end);
    free(left_t);
    free(right_t);
    free(car_tail);

    return 0;
}

void *monitor(){


	pthread_mutex_lock(&mutex);
	if(curr_dir=='l'){
		pthread_cond_signal(&left);
		pthread_cond_wait(&monitor_c, &mutex);
	}else{
		pthread_cond_signal(&right);
		pthread_cond_wait(&monitor_c, &mutex);
	}
	pthread_mutex_unlock(&mutex);

	pthread_mutex_unlock(&end);
	return NULL;

}

void *left_car( void *i){
 	
	int j;
	j=(intptr_t)i;

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&left, &mutex);
	pthread_mutex_unlock(&mutex);
	printf("Hi i am left\n");
	pthread_cond_signal(&left);

	return NULL;

}                   
void *right_car( void *i){
 	
	int j;
	j=(intptr_t)i;
               
	pthread_mutex_lock(&mutex);
        pthread_cond_wait(&right, &mutex);
	pthread_mutex_unlock(&mutex);
	printf("Hi i am right\n");
	pthread_cond_signal(&right);

	return NULL;
}                   
