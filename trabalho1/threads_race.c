#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 5
#define TARGET 99

// Variaveis
int winner = -1;
pthread_mutex_t mutex;
pthread_cond_t start_cond;
int threads_ready = 0;

// Funcao que define o comportamento de cada thread
void* thread_func(void* arg) {
    int id = *(int*)arg;
    free(arg);

    int local_counter = 0;

    // Cada thread fica esperando ate que todas as outras estejam prontas para comecar
    pthread_mutex_lock(&mutex);
    threads_ready++;
    while (threads_ready != NUM_THREADS){
    	pthread_cond_wait(&start_cond, &mutex);
    }
    pthread_cond_broadcast(&start_cond);
    pthread_mutex_unlock(&mutex);

    while (local_counter <= TARGET) {
        pthread_mutex_lock(&mutex);

        // Verifica se alguma thread ja ganhou
        if (winner != -1) {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        if (rand() % 5 == 0) { // Simula uma chance (20%) de perda de contagem
            local_counter--;
            if (local_counter < 0) {
                local_counter = 0;
            }
            printf("Thread %d: perdeu um ponto, contador local = %d\n", id, local_counter);
        } else {
            local_counter++;
            printf("Thread %d: contador local = %d\n", id, local_counter);
        }

        // Verifica se a thread atingiu o alvo
        if (local_counter == TARGET) {
            winner = id;
            printf("Thread %d ganhou a corrida!\n", id);
        }
        
        pthread_mutex_unlock(&mutex);
        
        // Da uma pausa para permitir que outras threads concorram
        usleep(10000);
    }

    return NULL;
}

// Função principal
int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&start_cond, NULL);

    // Inicializa a semente do gerador de numeros aleatorios
    srand(time(NULL));

    // Criacao das threads
    for (int i = 0; i < NUM_THREADS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, thread_func, id);
    }

    // Thread princiapl espera as threads terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&start_cond);

    return  0;
}