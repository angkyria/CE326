#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


void *train();

void * passenger();

int train_tail, last_train_capacity,noboard, train_capacity, flag;
pthread_mutex_t mtx_boarding, mtx_start, mtx_end, mutex;
pthread_cond_t pass_c, start_c, train_c;


int main (int argc, char* argv[]){
    int i, num_passengers, t_status, capacity, mtx_status, cond_status;
    pthread_t *t_passengers, t_train;

				noboard=0;
    flag=0;

    printf ("Give train capacity: ");
    scanf (" %d", &capacity);
    printf ("How many passengers to boarding: ");
    scanf (" %d", &num_passengers);

    train_tail= num_passengers/capacity;
    last_train_capacity= num_passengers%capacity;
    train_capacity= capacity;

    mtx_status=pthread_mutex_init(&mtx_end, NULL);
    if (mtx_status) {
        perror("Fail create mutex\n");
        exit(1);
    }

    mtx_status=pthread_mutex_init(&mtx_start, NULL);
    if (mtx_status) {
        perror("Fail create mutex\n");
        exit(1);
    }

    mtx_status=pthread_mutex_init(&mtx_boarding, NULL) ;
    if (mtx_status) {
        perror("Fail create mutex\n");
        exit(1);
    }

    mtx_status=pthread_mutex_init(&mutex, NULL);
    if (mtx_status) {
        perror("Fail create mutex\n");
        exit(1);
    }

    cond_status=pthread_cond_init(&pass_c,NULL);
     if (cond_status) {
        perror("Fail create cond\n");
        exit(1);
    }        
    cond_status=pthread_cond_init(&pass_c,NULL);
     if (cond_status) {
        perror("Fail create cond\n");
        exit(1);
    }        
     cond_status=pthread_cond_init(&train_c,NULL);
     if (cond_status) {
        perror("Fail create cond\n");
        exit(1);
    } 
    t_passengers= (pthread_t*)malloc(sizeof(pthread_t)*(num_passengers+1));
    if (t_passengers==NULL) {
        perror("Fail allocation memory");
        exit(2);
    }

    for (i=0; i<num_passengers; i++){

        t_status = pthread_create( &t_passengers[i], NULL, passenger , NULL);
        if(t_status){
            perror("Fail create thread passengers");
            exit(3);
        }

    }
 
    t_status = pthread_create( &t_train, NULL, train , NULL);
    if(t_status){
        perror("Fail create thread train");
        exit(3);
    }                                  

    pthread_mutex_lock (&mtx_end);


    printf("!!!!!\n");

    pthread_mutex_destroy (&mtx_end);
    pthread_mutex_destroy (&mtx_boarding);
    pthread_mutex_destroy (&mutex);
    pthread_mutex_destroy (&mtx_start);
    pthread_cond_destroy(&pass_c);
    pthread_cond_destroy(&start_c);
    pthread_cond_destroy(&train_c);  

    free (t_passengers);
    return 0;

}


void * train(){
	pthread_mutex_lock(&mutex);
        printf("Start boarding\n");
	pthread_cond_signal(&pass_c);
	pthread_cond_wait(&start_c, &mutex);
	pthread_mutex_unlock(&mutex);

    while (1){

        pthread_mutex_lock (&mtx_start);

        printf ("The train Start\n");
        printf ("Board passengers %d\n", train_capacity);
	sleep(1);

        train_tail--;


        if ((train_tail==0)&&(last_train_capacity==0)){
            printf ("No one else to board the stasion close\n");
            pthread_mutex_unlock (&mtx_end);
	    printf("Station close");
            break;

        }

        if(flag==1){
            printf ("No one else to board the stasion close\n");
            pthread_mutex_unlock (&mtx_end);
	    printf("Stasion close");
            break;
        }

        else{
            printf ("Going at the start line\n");
            pthread_cond_signal(&pass_c);
	    
        }
         pthread_cond_wait(&start_c, &mtx_start);
	    pthread_mutex_unlock(&mtx_start);
             
    }

    return NULL;
}

void *passenger(){

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&pass_c, &mutex);
	pthread_mutex_unlock(&mutex);

    pthread_mutex_lock (&mtx_boarding);
    if ((train_tail==0)&&(last_train_capacity!=0)&&(flag==0)){
        printf("Last wagon of the train\n");
	sleep(1);
        train_capacity=last_train_capacity;
        flag=1;
    }

    noboard++;

    if (noboard!=train_capacity){
        printf("..");
	sleep(1);
        pthread_cond_signal (&pass_c);
    }else {
        pthread_cond_signal(&train_c);
    }

    pthread_cond_wait(&train_c, &mtx_boarding);
    sleep(1);
    noboard--;
    
    if (noboard==0){
        printf("\nFinish boarding\n");
	sleep(2);
        pthread_cond_signal (&start_c);
    }else{
        pthread_cond_signal(&train_c);

    }
    pthread_mutex_unlock(&mtx_boarding);
    
    return NULL;
    
}
