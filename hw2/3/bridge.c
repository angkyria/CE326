#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#define RIGHT_PASS 10

void *car_arrive( void *i);
void car_cross(char direction);
void car_leave(char direction);

char *car_tail, curr_dir;
int size_of_car_tail, size_of_left_tail, size_of_rigth_tail, passed_car, priority, bridge_capacity, car_on_bridge, car_right_pass, flag, empty_l_tail, empty_r_tail;
pthread_mutex_t mtxend, mtx_dir_pri, mtx_r_dir, mtx_l_dir, mtxleave;

int main(int argc, char *argv[]){

    char c;
    int i, thread_status, mtx_status;
    pthread_t *car;

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
            car_tail[i] = 'l';
            size_of_left_tail++;
            i++;

        }

    }while(c!='\n');

    printf("Give the maximum number of cars over the bridge: ");
    scanf(" %d", &bridge_capacity);
    size_of_car_tail = i;
    passed_car = 0;
    car_on_bridge = 0;
    car_right_pass = 0;
    priority=0;
    empty_l_tail=0;
    empty_r_tail=0;

    mtx_status = pthread_mutex_init(&mtxend, NULL);//init mutex end of car tail
    if(mtx_status){
        perror("Fail init mutex end\n");
        exit(1);
    }

    mtx_status = pthread_mutex_init(&mtx_dir_pri, NULL);//init mutex car direction priority
    if(mtx_status){
        pthread_mutex_destroy(&mtxend);
        free(car_tail);
        perror("Fail init mutex priority direction\n");
        exit(1);
    }

    mtx_status = pthread_mutex_init(&mtx_l_dir, NULL);//init mutex car line left direction
    if(mtx_status){
        pthread_mutex_destroy(&mtxend);
        pthread_mutex_destroy(&mtx_dir_pri);
        free(car_tail);
        perror("Fail init mutex left direction\n");
        exit(1);
    }

    mtx_status = pthread_mutex_init(&mtx_r_dir, NULL);//init mutex car line right direction
    if(mtx_status){
        pthread_mutex_destroy(&mtxend);
        pthread_mutex_destroy(&mtx_dir_pri);
        pthread_mutex_destroy(&mtx_l_dir);
        free(car_tail);
        perror("Fail init mutex right direction\n");
        exit(1);
    }

    mtx_status = pthread_mutex_init(&mtxleave, NULL);//init mutex car leave from bridge
    if(mtx_status){
        pthread_mutex_destroy(&mtxend);
        pthread_mutex_destroy(&mtx_dir_pri);
        pthread_mutex_destroy(&mtx_l_dir);
        pthread_mutex_destroy(&mtx_r_dir);
        free(car_tail);
        perror("Fail init mutex car leave bridge\n");
        exit(1);
    }

    car = (pthread_t *) malloc(size_of_car_tail*sizeof(pthread_t));//allocate number of car threads
    for(i=0;i<size_of_car_tail;i++){
        thread_status=pthread_create(&car[i],NULL, car_arrive, (void*)(intptr_t)i);//start n car thread
        if(thread_status){
            perror("Fail create thread\n");
            exit(2);
        }
    }

    pthread_mutex_lock(&mtxend);//lock until pass all the cars from the bridge

    sleep(5);

    pthread_mutex_destroy(&mtxend);
    pthread_mutex_destroy(&mtx_dir_pri);
    pthread_mutex_destroy(&mtx_l_dir);
    pthread_mutex_destroy(&mtx_r_dir);
    pthread_mutex_destroy(&mtxleave);
    free(car);
    free(car_tail);

    return 0;

}

void *car_arrive(void *i){
    int j;
    j=(intptr_t)i;

    pthread_mutex_lock(&mtx_dir_pri);
    if(priority == 0){//first car on the line take the priority

        if(car_tail[0]=='r'){//block opposite direction
            curr_dir='r';
            pthread_mutex_lock(&mtx_l_dir);
        }else{
            curr_dir='l';
            pthread_mutex_lock(&mtx_r_dir);
        }

        priority=1;

    }
    pthread_mutex_unlock(&mtx_dir_pri);

    if(car_tail[j]=='r')//
        pthread_mutex_lock(&mtx_r_dir);
    else
        pthread_mutex_lock(&mtx_l_dir);

    car_cross(car_tail[j]);
    car_leave(car_tail[j]);
    if((passed_car == size_of_car_tail) && (size_of_left_tail==0) && (size_of_rigth_tail==0))pthread_mutex_unlock(&mtxend);

    return NULL;
}

void car_cross(char direction){

    car_on_bridge++;
    car_right_pass++;
    printf("%c car crossing the bridge\n", direction);
    if((car_on_bridge=!bridge_capacity) && (curr_dir=='r'))
        pthread_mutex_unlock(&mtx_r_dir);
    if((car_on_bridge=!bridge_capacity) && (curr_dir=='l'))
        pthread_mutex_unlock(&mtx_l_dir);

}

void car_leave(char direction){

    pthread_mutex_lock(&mtxleave);
    printf("%c car leaving the bridge\n", direction);
    passed_car++;

    if(direction=='r')
        size_of_rigth_tail--;
    else
        size_of_left_tail--;

    if(size_of_left_tail==0){//we dont have cars on left direction
        if(empty_l_tail==0){
            if(size_of_rigth_tail!=0)printf("Only the right side have cars\n");
            empty_l_tail=1;
        }
        curr_dir='r';
        flag=0;

    }else if(size_of_rigth_tail==0){// we dont have cars on right direction
        if(empty_r_tail==0){
            if(size_of_left_tail!=0)printf("Only the left side have cars\n");
            empty_r_tail=1;
        }
        curr_dir='l';
        flag=0;

    }else
        flag=1;

    if(flag==1){

        if((car_right_pass==RIGHT_PASS) && (curr_dir=='r')){

            printf("Change direction from right to left\n");
            curr_dir='l';
            car_on_bridge=0;
            car_right_pass=0;
            pthread_mutex_unlock(&mtx_l_dir);

        }

        if((car_right_pass==RIGHT_PASS) & (curr_dir=='l')){

            printf("Change direction from left to right\n");
            curr_dir='r';
            car_on_bridge=0;
            car_right_pass=0;
            pthread_mutex_unlock(&mtx_r_dir);

        }

        if(car_on_bridge==bridge_capacity){

            printf("Bridge full start empty\n");
            car_on_bridge=0;
            if(curr_dir=='l')
                pthread_mutex_unlock(&mtx_l_dir);
            else
                pthread_mutex_unlock(&mtx_r_dir);

        }

        if(car_on_bridge!=RIGHT_PASS){

            if(curr_dir=='l')
                pthread_mutex_unlock(&mtx_l_dir);
            else
                pthread_mutex_unlock(&mtx_r_dir);

        }

    }

    if(flag==0){


        if(car_on_bridge==bridge_capacity){
            printf("Bridge full start empty\n");
            car_on_bridge=0;
            if(curr_dir=='l')
                pthread_mutex_unlock(&mtx_l_dir);
            else
                pthread_mutex_unlock(&mtx_r_dir);
            
        }else
            
            if(curr_dir=='l')
                pthread_mutex_unlock(&mtx_l_dir);
            else
                pthread_mutex_unlock(&mtx_r_dir);
        
    }
    
    pthread_mutex_unlock(&mtxleave);
}
