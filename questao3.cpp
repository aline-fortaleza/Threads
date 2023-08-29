#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Iniciando Leitores e Escritores
int n;
int m;

// Definindo o tamanho do dado
#define DATA_SIZE 10
int data[DATA_SIZE];

// Inicializando o Mutex e Cond
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_leitor = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_escritor = PTHREAD_COND_INITIALIZER;

// Inicializando as variaveis globais
int count_leitores = 0;
int wait_escritores = 0;
int aberto = 0;

// FunÃ§Ã£o do Leitor para ler os dados da data[]
void* ler(void* arg) {

    // Trazendo o id da Thread
    int thread_id = *(int*)arg;

    // Loop Infinito
    while (1) {

        // Dando lock no Mutex
        pthread_mutex_lock(&mutex);

        // Esperando ser liberado para leitura
        while (aberto || wait_escritores > 0) {

            pthread_cond_wait(&cond_leitor, &mutex);

        }

        // Avisando que estÃ¡ lendo os dados
        count_leitores++;
        pthread_mutex_unlock(&mutex);

        printf("Leitor %d lendo data: ", thread_id);

        // Lendo os dados de data
        for (int i = 0; i < DATA_SIZE; i++) {

            printf("%d ", data[i]);

        }

        printf("\n");

        // Retirando do contador de leitores e informando que nÃ£o estÃ¡ lendo mais
        pthread_mutex_lock(&mutex);
        count_leitores--;

        // Liberando o cond
        if (count_leitores == 0) {

            pthread_cond_signal(&cond_escritor);

        }

        // Dando unlock no mutex
        pthread_mutex_unlock(&mutex);

        // Esperando para rodar novamente
        sleep(thread_id + 2);

    }

    return NULL;
}

void* escrever(void* arg) {

    // Trazendo o id da Thread
    int thread_id = *(int*)arg;


    // Loop Infinito
    while (1) {

        // Dando lock no Mutex
        pthread_mutex_lock(&mutex);

        // Incrementando o contador de escritores
        wait_escritores++;

        // Esperando ser liberado para escrita
        while (count_leitores > 0 || aberto) {

            pthread_cond_wait(&cond_escritor, &mutex);

        }

        // Abrindo a ediÃ§Ã£o
        aberto = 1;
        wait_escritores--;

        printf("Escritor %d escrevendo data\n", thread_id);

        // Editando os dados de data[]
        for (int i = 0; i < DATA_SIZE; i++) {

            data[i] = data[i] + i; 

        }
        // Liberando o aberto e diminuindo 1 do contador de escritor
        aberto = 0;

        // Informando que ta tudo ok, pode voltar a ler os dados
        pthread_cond_broadcast(&cond_leitor);
        pthread_cond_signal(&cond_escritor);

        // Dando Unlock no Mutex
        pthread_mutex_unlock(&mutex);

        // Aguardando para repetir tudo novamente
        sleep(thread_id + 2);
    }

    return NULL;
}

// Main do cÃ³digo
int main() {

    printf("\n");
    printf("-#-#- Inicializando o Programa da QuestÃ£o 3 -#-#-\n");
    printf("\n");

    printf("Escolha o Numero de Leitores e Escritores / (N) e (M):\n");

    scanf("%d %d", &n, &m);

    // Iniciando as Threads
    pthread_t leitores[n];
    pthread_t escritores[m];

    // Criando os Leitores, cada leitor Ã© uma thread separada
    for (int i = 0; i < n; i++) {

        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i + 1;
        pthread_create(&leitores[i], NULL, ler, (void*)thread_id);

    }

    // Criando os escritores, cada escritor Ã© uma thread separada
    for (int i = 0; i < m; i++) {

        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i + 1;
        pthread_create(&escritores[i], NULL, escrever, (void*)thread_id);

    }

    // Dando join nos leitores quando acabar
    for (int i = 0; i < n; i++) {

        pthread_join(leitores[i], NULL);

    }

    // Dando join nos escritores quando acabar
    for (int i = 0; i < m; i++) {

        pthread_join(escritores[i], NULL);

    }

    return 0;

}

