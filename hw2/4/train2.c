#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>


void *train();

void * passenger();


int train_tail, last_train_capacity,noboard, train_capacity, flag;
pthread_mutex_t mtx_boarding, mtx_board, mtx_end, mtx_ready, mtx_start;




int main (int argc, char* argv[]){
    int i, num_passengers, t_status, capacity, mtx_status;
    pthread_t *t_passengers, t_train;

				noboard=0;
    flag=0;

    printf ("Give train capacity: ");
    scanf (" %d", &capacity);
    printf ("How many passengers to boarding: ");
    scanf (" %d", &num_passengers);

    train_tail= num_passengers/capacity;			//poses fores tha ksekinisei to treno (o pragmatikos arithmos einai train_tail+1)
    last_train_capacity= num_passengers%capacity;			//posoi epivates perisevoun sto teleytaio treno
    train_capacity= capacity;				//xwritikotita trenou

    mtx_status=pthread_mutex_init(&mtx_end, NULL);
    if (mtx_status) {
        perror("Fail create mutex\n");
        exit(1);
    }

    mtx_status=pthread_mutex_init(&mtx_ready, NULL);
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

    mtx_status=pthread_mutex_init(&mtx_board, NULL);
    if (mtx_status) {
        perror("Fail create mutex\n");
        exit(1);
    }

    pthread_mutex_lock (&mtx_end);
    pthread_mutex_lock (&mtx_ready);
    pthread_mutex_lock (&mtx_start);
    pthread_mutex_lock (&mtx_board);


    t_status = pthread_create( &t_train, NULL, train , NULL);
    if(t_status){
        perror("Fail create thread train");
        exit(3);
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


    pthread_mutex_lock (&mtx_end);	//blokarisma mexri na oloklirwthei i diadikasia

    pthread_mutex_destroy (&mtx_end);	//diagrafi twn mutexes
    pthread_mutex_destroy (&mtx_boarding);
    pthread_mutex_destroy (&mtx_board);
    pthread_mutex_destroy (&mtx_ready);
    pthread_mutex_destroy (&mtx_start);

    free (t_passengers);		//apeleftherwsi xwrou tou pinaka twn threads
    return (0);

}


void * train(){

    while (1){
        printf("Start boarding\n");
        pthread_mutex_lock (&mtx_start);


								printf ("The train Start\n");
        printf ("Board passengers %d\n", train_capacity);

        train_tail--;


        if ((train_tail==0)&&(last_train_capacity==0)){
            printf ("No one else to board the stasion close\n");
            pthread_mutex_unlock (&mtx_end);
            break;

        }

        if(flag==1){
            printf ("No one else to board the stasion close\n");
            pthread_mutex_unlock (&mtx_end);
            break;
        }

        else{
            printf ("Going at the start line\n");
            pthread_mutex_unlock (&mtx_boarding);

        }

    }

    return NULL;
}

void *passenger(){

    pthread_mutex_lock (&mtx_boarding);
    if ((train_tail==0)&&(last_train_capacity!=0)&&(flag==0)){
        printf("Last wagon of the train\n");
        train_capacity=last_train_capacity;
        flag=1;
    }

    noboard++;

    if (noboard!=train_capacity){
        printf("..");
        pthread_mutex_unlock (&mtx_boarding);
    }else {
        pthread_mutex_unlock(&mtx_board);
    }

    pthread_mutex_lock(&mtx_board);

    noboard--;
    
    if (noboard==0){
        printf("\nFinish boarding\n");
        pthread_mutex_unlock (&mtx_start);
    }else{
        pthread_mutex_unlock (&mtx_board);
    }
    
    return NULL;

}




