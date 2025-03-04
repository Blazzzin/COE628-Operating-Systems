#include <stdio.h> /* printf */
#include <stdlib.h> /* exit, EXIT_FAILURE, random */
#include <pthread.h> /* pthread functions */
#include <unistd.h> /* getpid, usleep */
#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* wait */

void *count(void *); // Function prototype
int globalNumber = 0; // Shared variable
pthread_mutex_t mutex1; // Mutex

int main(void) {
    int i;
    pthread_t counterThread[5];

    // Initialize the mutex
    pthread_mutex_init(&mutex1, NULL);

    // Create the 5 threads
    for (i = 0; i < 5; i++) {   
        if (pthread_create(&counterThread[i], NULL, count, NULL) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (i = 0; i < 5; i++) {
        pthread_join(counterThread[i], NULL);
    }

    // Destroy the mutex
    pthread_mutex_destroy(&mutex1);

    printf("Final value of globalNumber: %d\n", globalNumber);

    return 0;
}

void *count(void *junk) {
    int loopCount = 0;

    while (loopCount < 10) {
        pthread_mutex_lock(&mutex1); // Lock before modifying shared variable
        int tmpNumber = globalNumber;
        printf("counter: %d, Thread: %ld, PID: %d\n",
               tmpNumber, pthread_self(), getpid());
        tmpNumber++;
        usleep(random() % 2);
        globalNumber = tmpNumber;
        pthread_mutex_unlock(&mutex1); // Unlock after modifying
        loopCount++;
    }

    return NULL;
}