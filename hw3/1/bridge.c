#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#define RIGHT_PASS 5

void *left_car();
void *right_car();
void *monitor();
void car_cross( char direction );
void car_leave( char direction );

pthread_mutex_t end, mutex, left_m, right_m;
pthread_cond_t left, right, monitor_c;

char *car_tail, curr_dir;
int size_of_tail, size_of_left_tail, size_of_rigth_tail, bridge_capacity, passed_car, passed_right, passed_left, car_on_bridge, car_right_pass,change_dir;



int main(int argc, char *argv[]){

    pthread_t monitor_t, *left_t, *right_t;
    char c;
    int i, thread_status,mtx_status, cond_status;

    printf("Give the cars directions. L or l for the left and R or r for the right direction: ");
    i=0; size_of_left_tail=0; size_of_rigth_tail=0;passed_left=0;passed_right=0;passed_car=0;car_on_bridge=0;car_right_pass=0;change_dir=0;
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

    mtx_status=pthread_mutex_init(&left_m, NULL);
    if(mtx_status){
        perror("Fail init mutex end\n");
        exit(1);
    }

    mtx_status=pthread_mutex_init(&right_m, NULL);
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

            thread_status=pthread_create(&left_t[i], NULL, left_car, NULL);
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
        for(i=0;i<size_of_rigth_tail;i++){

            thread_status=pthread_create(&right_t[i], NULL, right_car,NULL);
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
    if(size_of_left_tail!=0)free(left_t);
    if(size_of_rigth_tail!=0)free(right_t);
    free(car_tail);

    return 0;
}

void *monitor(){
    int flag, check;
    check=0;

    pthread_mutex_lock(&mutex);
    if(curr_dir=='l'){
        pthread_cond_signal(&left);
        pthread_cond_wait(&monitor_c, &mutex);
    }else{
        pthread_cond_signal(&right);
        pthread_cond_wait(&monitor_c, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    while(passed_car!=size_of_tail){


        pthread_mutex_lock(&mutex);
        if(size_of_left_tail==passed_left)
            flag=1;
        else if (size_of_rigth_tail==passed_right)
            flag=2;
        else
            flag=0;
    	if(change_dir==1){
        if(flag==0){

            if(curr_dir=='l'){
                printf("Change direction from left to right\n");
                curr_dir='r';
                car_on_bridge=0;
                car_right_pass=0;
                pthread_cond_signal(&right);
                pthread_cond_wait(&monitor_c, &mutex);
            }else if(curr_dir=='r'){
                printf("Change directions from right to left\n");
                curr_dir='l';
                car_on_bridge=0;
                car_right_pass=0;
                pthread_cond_signal(&left);
                pthread_cond_wait(&monitor_c, &mutex);
            }
        }
	}

        if((flag==1)&&(check==0)){
		check=1;
		printf("We have car only at the right\n");
		pthread_cond_signal(&right);
	}
	if((flag==2)&&(check==0)){
		check=1;
		printf("We have car only at the left\n");
		pthread_cond_signal(&left);
	}
         
         
        pthread_mutex_unlock(&mutex);

    }


    pthread_mutex_unlock(&end);
    return NULL;

}

void *left_car(){


    pthread_mutex_lock(&left_m);
    pthread_cond_wait(&left, &left_m);
    pthread_mutex_unlock(&left_m);

    sleep(1);
    car_cross('l');
    car_leave('l');
    return NULL;

}
void *right_car(){


    pthread_mutex_lock(&right_m);
    pthread_cond_wait(&right, &right_m);
    pthread_mutex_unlock(&right_m);
    sleep(1);
    car_cross('r');
    car_leave('r');
    return NULL;
}


void car_cross(char dir){

    pthread_mutex_lock(&mutex);
    car_on_bridge++;
    car_right_pass++;
    printf("%c car crossing the bridge\n", dir);
    if((car_on_bridge!=bridge_capacity) && (curr_dir='l')){
        pthread_cond_signal(&left);
    }
    if((car_on_bridge!=bridge_capacity) && (curr_dir='r')){
        pthread_cond_signal(&right);
    }  
    
    
}

void car_leave(char dir){
    
    printf("%c car leaving the bridge\n", dir);
    passed_car++;
    if(dir=='l')
        passed_left++;
    else
        passed_right++;
    
    
    if(bridge_capacity==car_on_bridge){
        printf("Bridge full\n");
        car_on_bridge=0;
        if(dir=='l'){
            pthread_cond_signal(&left);
        }else{
            pthread_cond_signal(&right);
        }
    }
    if(car_right_pass==RIGHT_PASS){
    	    change_dir=1;
	    pthread_cond_signal(&monitor_c);
    }else{
	    change_dir=0;
	    pthread_cond_signal(&monitor_c);
    }
    pthread_mutex_unlock(&mutex);
}
