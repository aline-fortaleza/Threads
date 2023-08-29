#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Definindo a quantidade de Leitores e Escritores
#define NUM_LEITORES 5
#define NUM_ESCRITORES 2

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

// Função do Leitor para ler os dados da data[]
void* reader(void* arg) {

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

        // Adicionando ao contador de leitura
        count_leitores++;

        printf("Leitor %d lendo data: ", thread_id);

        // Lendo os dados de data
        for (int i = 0; i < DATA_SIZE; i++) {

            printf("%d ", data[i]);

        }

        printf("\n");

        // Retirando do contador de leitores
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

void* writer(void* arg) {

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

        // Abrindo a edição
        aberto = 1;

        printf("Escritor %d escrevendo data\n", thread_id);

        // Editando os dados de data[]
        for (int i = 0; i < DATA_SIZE; i++) {

            data[i] = data[i] + i; 

        }

        // Liberando o aberto e diminuindo 1 do contador de escritor
        wait_escritores--;
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

// Main do código
int main() {

    printf("\n");
    printf("-#-#- Inicializando o Programa da Questão 3 -#-#-\n");
    printf("\n");

    // Iniciando as Threads
    pthread_t leitores[NUM_LEITORES];
    pthread_t escritores[NUM_ESCRITORES];

    // Criando os Leitores, cada leitor é uma thread separada
    for (int i = 0; i < NUM_LEITORES; i++) {

        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i + 1;
        pthread_create(&leitores[i], NULL, reader, (void*)thread_id);

    }

    // Criando os escritores, cada escritor é uma thread separada
    for (int i = 0; i < NUM_ESCRITORES; i++) {

        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i + 1;
        pthread_create(&escritores[i], NULL, writer, (void*)thread_id);

    }

    // Dando join nos leitores quando acabar
    for (int i = 0; i < NUM_LEITORES; i++) {

        pthread_join(leitores[i], NULL);

    }

    // Dando join nos escritores quando acabar
    for (int i = 0; i < NUM_ESCRITORES; i++) {

        pthread_join(escritores[i], NULL);

    }

    return 0;

}
