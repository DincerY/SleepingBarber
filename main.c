#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define CUSTOMER 5

sem_t barber_wait;
sem_t customer_wait;
sem_t customer_chair;


void *barber_func(void *arg) {
    while (1)
    {
        sem_wait(&customer_wait);
        printf("Barber work\n");
        sleep(5);
        printf("Haircut finish\n");
        sem_post(&barber_wait);
    }
        
    return NULL;
}

void *customer_func(void *arg) {
    printf("%d . Customer come\n",*(int*)arg);
    sem_wait(&customer_chair);
    printf("%d . Customer in waiting room\n",*(int*)arg);
    sem_post(&customer_wait);
    sem_wait(&barber_wait);
    sem_post(&customer_chair);
    free(arg);
    return NULL;
}

int main() {
    sem_init(&barber_wait,0,0);
    sem_init(&customer_wait,0,0);
    sem_init(&customer_chair,0,3);
    pthread_t barber,customer[CUSTOMER];

    pthread_create(&barber,NULL,barber_func,NULL);

    for (int i = 0; i < CUSTOMER; i++)
    {
        int *a = malloc(sizeof(int));
        *a = i+1;
        pthread_create(&customer[i],NULL,customer_func,a);
    }
    
    for (int i = 0; i < CUSTOMER; i++)
    {
        pthread_join(customer[i],NULL);
    }

    pthread_cancel(barber); 
    pthread_join(barber, NULL);
    
    sem_destroy(&barber_wait);
    sem_destroy(&customer_wait);
    sem_destroy(&customer_chair);
    return 0;
}