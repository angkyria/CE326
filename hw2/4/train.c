#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


void *train ();
void *passengers ();

int train_capacity, last_train_capacity, train_trails, flag, noboard;
pthread_mutex_t mtx_end, mtx_board, mtx_entry, mtx_ready, mtx_ride;

int main(int argc, char *argv[]){

    int i, thread_status, mtx_status, num_passengers;
    pthread_t t_train;
    pthread_t *t_passengers;

    printf("Give the capacity of the train: ");
    scanf(" %d", &train_capacity);

    printf("Give how many passengers: ");
    scanf(" %d", &num_passengers);

    train_trails=num_passengers/train_capacity;
    last_train_capacity=num_passengers%train_capacity;
    noboard=0;
    flag=0;


    mtx_status=pthread_mutex_init(&mtx_end, NULL);
    if(mtx_status){
        perror("Fail to create end mutex\n");
        exit (1);
    }

    mtx_status=pthread_mutex_init(&mtx_board, NULL);
    if(mtx_status){
        perror("Fail to create end mutex\n");
        exit (1);
    }

    mtx_status=pthread_mutex_init(&mtx_entry, NULL);
    if(mtx_status){
        perror("Fail to create end mutex\n");
        exit (1);
    }

    mtx_status=pthread_mutex_init(&mtx_ready, NULL);
    if(mtx_status){
        perror("Fail to create end mutex\n");
        exit (1);
    }

    mtx_status=pthread_mutex_init(&mtx_ride, NULL);
    if(mtx_status){
        perror("Fail to create end mutex\n");
        exit (1);
    }

    //pthread_mutex_lock(&mtx_board);
    //pthread_mutex_lock(&mtx_entry);
    //pthread_mutex_lock(&mtx_ready);
    //pthread_mutex_lock(&mtx_ride);

    thread_status=pthread_create(&t_train, NULL, train, NULL);
    if (thread_status){
        perror("Fail to create train thread");
        exit (2);
    }
    t_passengers=(pthread_t *) malloc(num_passengers*sizeof(pthread_t));
    if (t_passengers==NULL){
        perror("Error allocate passengers memory");
        exit (3);
    }

    for(i=0;i<num_passengers;i++){
        thread_status=pthread_create(&t_passengers[i], NULL, passengers, NULL);
        if(thread_status){
            perror("Fail to create passengers treads\n");
            exit(4);
        }
    }


    pthread_mutex_lock(&mtx_end);

    pthread_mutex_destroy(&mtx_end);
    pthread_mutex_destroy(&mtx_board);
    pthread_mutex_destroy(&mtx_entry);
    pthread_mutex_destroy(&mtx_ready);
    pthread_mutex_destroy(&mtx_ride);
    free(t_passengers);


    return 0;
}

void *train(){

    while(1){
	    printf("Start boarding\n");
        pthread_mutex_lock(&mtx_ride);

        printf("The train start\n");
        printf("Board passengers %d\n", train_capacity);
	printf("__---__---__---__\n");
        printf("End of ther ride\n");
        train_trails--;


        if((train_trails==0)&&(last_train_capacity==0)){
            printf("No more passengers the train close\n");
            pthread_mutex_unlock(&mtx_end);
            break;
        } 

        if(flag == 1){
            printf("No more passengers the train close\n");
            pthread_mutex_unlock(&mtx_end);
            break;
        }else{
		noboard=0;
            printf("Going to the start line to board passengers\n");

        }

    }

    return NULL;
}


void *passengers(){

    pthread_mutex_lock(&mtx_entry);
    if((train_trails==0)&&(last_train_capacity!=0)&&(flag==0)){
	printf("Boarding of the last wagon train\n");
        train_capacity=last_train_capacity;
        flag=1;
    }
    
    noboard++;
    if(noboard!=train_capacity){
        pthread_mutex_unlock(&mtx_entry);
    }else{
        pthread_mutex_unlock(&mtx_board);
    }
    
    pthread_mutex_lock(&mtx_board);
    noboard--;
    if(noboard==0){
        pthread_mutex_unlock(&mtx_ride);
    }
    if (noboard!=0){
        pthread_mutex_unlock(&mtx_board);
    }
    
    return NULL;
    
}
