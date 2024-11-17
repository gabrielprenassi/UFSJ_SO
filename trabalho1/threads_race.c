#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 5
#define TARGET 99

// Variaveis
int vencedor = -1;
pthread_mutex_t mutex;
pthread_cond_t cond;

// Funcao que define o comportamento de cada thread
void* thread_func(void* arg) {
    int id = *(int*)arg;
    free(arg);

    int contador_local = 0;

    while (contador_local <= TARGET) {
        pthread_mutex_lock(&mutex);

        // Verifica se alguma thread ja ganhou
        if (vencedor != -1) {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }

        // Simula uma chance (20%) de perda de contagem
        if (rand() % 5 == 0) {
            contador_local--;
            if (contador_local < 0) {
                contador_local = 0;
            }
            printf("Thread %d: perdeu um ponto, contador local = %d\n", id, contador_local);
        } else {
            contador_local++;
            printf("Thread %d: contador local = %d\n", id, contador_local);
        }

        // Verifica se a thread atingiu o alvo
        if (contador_local == TARGET) {
            vencedor = id;
            printf("Thread %d ganhou a corrida!\n", id);
            pthread_cond_broadcast(&cond);
        }
        
        pthread_mutex_unlock(&mutex);
        
        // Da uma pausa para permitir que outras threads concorram
        usleep(10000);
    }

    return NULL;
}

// Funcao principal
int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Inicializa a semente do gerador de numeros aleatorios
    srand(time(NULL));

    // Criacao das threads
    for (int i = 0; i < NUM_THREADS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, thread_func, id);
    }

    // Thread principal aguarda o termino da corrida
    pthread_mutex_lock(&mutex);
    while (vencedor == -1) {
        pthread_cond_wait(&cond, &mutex); // Thread principal acorda quando a variavel vencedor for alterada
    }
    pthread_mutex_unlock(&mutex);

    // Thread principal espera as threads terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return  0;
}