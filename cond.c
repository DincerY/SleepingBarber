#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define CUSTOMER_COUNT 4

// Paylaşılan değişkenler
int customers = 0;
pthread_mutex_t mutex;
pthread_cond_t customer_cond, barber_cond;
sem_t waitingRoom;

// Berber fonksiyonu
void *barber(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (customers == 0) {
            printf("Berber uyuyor...\n");
            pthread_cond_wait(&barber_cond, &mutex);
        }
        printf("Berber müşteriyi kesiyor...\n");
        customers--;
        pthread_cond_signal(&customer_cond);
        pthread_mutex_unlock(&mutex);
        sleep(2); // Tıraş olma süresi
    }
    return NULL;
}

// Müşteri fonksiyonu
void *customer(void *arg) {
    sem_wait(&waitingRoom);
    pthread_mutex_lock(&mutex);
    customers++;
    printf("Müşteri geldi...\n");
    pthread_cond_signal(&barber_cond);
    while (customers > 0) {
        pthread_cond_wait(&customer_cond, &mutex);
    }
    printf("Müşteri tıraş oldu...\n");
    pthread_mutex_unlock(&mutex);
    sem_post(&waitingRoom);
    return NULL;
}

int main() {
    //daha farklı çözümlerinede bak iyice anla
    pthread_t barber_thread, customer_threads[CUSTOMER_COUNT];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&customer_cond, NULL);
    pthread_cond_init(&barber_cond, NULL);

    sem_init(&waitingRoom,0,3);

    pthread_create(&barber_thread, NULL, barber, NULL);

    for (int i = 0; i < CUSTOMER_COUNT; i++) {
        pthread_create(&customer_threads[i], NULL, customer, NULL);
    }

    for (int i = 0; i < CUSTOMER_COUNT; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    pthread_join(barber_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&customer_cond);
    pthread_cond_destroy(&barber_cond);

    return 0;
}