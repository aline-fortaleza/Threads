#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <ctime>
using namespace std;

// NÃºmero de Processos do Programa
#define NUM_PROCESSOS 6

// NÃºmero de Recursos do Programa
#define NUM_RECURSOS 6

// Definindo o grafo de alocaÃ§Ã£o de recursos
int grafo_pro_rec[NUM_PROCESSOS][NUM_RECURSOS];

// Int para rastrear nos visitados durante a busca
int pro_visitado[NUM_PROCESSOS];
int rec_visitado[NUM_RECURSOS];

int buscaEmProfundidade(int processo)
{
    // Marcando o processo como visitado
    pro_visitado[processo] = 1;

    // Procurando os recursos usados pelo processo
    for(int recurso = 0; recurso < NUM_RECURSOS; recurso++){

        // Caso encontre um recurso usado e que ainda nÃ£o tenha sido visitado
        if(grafo_pro_rec[processo][recurso] == 1 && rec_visitado[recurso] == 0){
            
            rec_visitado[recurso] = 1;

            // Busca pelos processos que esse recursos Ã© requisitado
            for(int pro_busca = 0; pro_busca < NUM_PROCESSOS; pro_busca++){

                // Verificar se outro processo jÃ¡ tinha bloqueado esse recurso
                if(grafo_pro_rec[pro_busca][recurso] == 1){

                    // Verificar se o processo jÃ¡ foi visitado
                    if(pro_visitado[pro_busca] == 1){       

                        if(pro_busca != processo){

                            printf("DeadLock encontrado no Processo %d e no Recurso %d\n", processo, recurso);

                        }

                    // Caso o processo nÃ£o tenha sido visitado
                    } else {

                        buscaEmProfundidade(pro_busca);
                    
                    }

                }

            }

        }

    }

}

void *buscar(void *arg)
{
    int noInicial = *((int *)arg);
    buscaEmProfundidade(noInicial);
    pthread_exit(NULL);
}

int main()
{       
    printf("\n");
    printf("-#-#- Inicializando o Programa da QuestÃ£o 2 -#-#-\n");
    printf("\n");
    
    // Iniciando as Threads e os nos para rodar
    pthread_t threads[NUM_PROCESSOS];
    int no[NUM_PROCESSOS];

    printf("-#- Inicializando Exemplo 1 -#-\n");
    printf("\n");

    // Zerando todos os valores de processos e recursos
    for(int processo = 0; processo < NUM_PROCESSOS; processo++){
        for(int recurso = 0; recurso < NUM_RECURSOS; recurso++){

            grafo_pro_rec[processo][recurso] = 0;

        }
    }

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 0
    grafo_pro_rec[0][4] = 1;
    grafo_pro_rec[0][5] = 1;
    printf("Processo 0 utiliza os recursos 4 e 5 \n");

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 1
    grafo_pro_rec[1][0] = 1;
    grafo_pro_rec[1][5] = 1;
    printf("Processo 1 utiliza os recursos 0 e 5 \n");

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 2
    grafo_pro_rec[2][1] = 1;
    grafo_pro_rec[2][2] = 1;
    grafo_pro_rec[2][3] = 1;
    printf("Processo 2 utiliza os recursos 1,2 e 3 \n");

    // processo 3 nÃ£o precisa de nenhum recurso
    printf("Processo 3 nÃ£o utiliza recursos \n");

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 4
    grafo_pro_rec[4][1] = 1;
    grafo_pro_rec[4][3] = 1;
    printf("Processo 4 utiliza os recursos 1 e 3 \n");

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 5
    grafo_pro_rec[5][0] = 1;
    grafo_pro_rec[5][4] = 1;
    printf("Processo 5 utiliza os recursos 0 e 4 \n");

    printf("\n");

    // Iniciando com 0 em todos os recursos
    for (int i = 0; i < NUM_RECURSOS; i++)
    {
        rec_visitado[i] = 0;
    }

    // Iniciando com 0 em todos os processos
    for (int i = 0; i < NUM_PROCESSOS; i++)
    {
        pro_visitado[i] = 0;
    }

    printf("Inicializando as Threads\n");
    // Criando as Threads iniciando a busca em cada um dos processos
    for (int i = 0; i < NUM_PROCESSOS; i++)
    {   
        printf("-#- Criando a Thread do Processo %d na main -#-\n", i);
        no[i] = i;
        pthread_create(&threads[i], NULL, buscar, &no[i]);
    }

    // Dando Join nas Threads e finalizando 
    for (int i = 0; i < NUM_PROCESSOS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("\n");
    printf("\n");
    printf("-#- Inicializando Exemplo 2 -#-\n");
    printf("\n");

    // Zerando todos os valores de processos e recursos
    for(int processo = 0; processo < NUM_PROCESSOS; processo++){
        for(int recurso = 0; recurso < NUM_RECURSOS; recurso++){

            grafo_pro_rec[processo][recurso] = 0;

        }
    }

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 0
    grafo_pro_rec[0][0] = 1;
    grafo_pro_rec[0][1] = 1;
    printf("Processo 0 utiliza os recursos 0 e 1 \n");

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 1
    grafo_pro_rec[1][1] = 1;
    grafo_pro_rec[1][2] = 1;
    printf("Processo 1 utiliza os recursos 1 e 2 \n");

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 2
    grafo_pro_rec[2][2] = 1;
    grafo_pro_rec[2][3] = 1;
    grafo_pro_rec[2][4] = 1;
    printf("Processo 2 utiliza os recursos 2,3 e 4 \n");

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 3
    grafo_pro_rec[2][4] = 1;
    grafo_pro_rec[2][5] = 1;
    printf("Processo 3 utiliza os recursos 4 e 5 \n");

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 4
    grafo_pro_rec[4][5] = 1;
    printf("Processo 4 utiliza os recursos 4 e 5 \n");

    // Colocando o valor (1) em alguns recursos utilizados pelo processo 5
    grafo_pro_rec[5][0] = 1;
    grafo_pro_rec[5][5] = 1;
    printf("Processo 5 utiliza os recursos 0 e 5 \n");

    printf("\n");

    // Iniciando com 0 em todos os recursos
    for (int i = 0; i < NUM_RECURSOS; i++)
    {
        rec_visitado[i] = 0;
    }

    // Iniciando com 0 em todos os processos
    for (int i = 0; i < NUM_PROCESSOS; i++)
    {
        pro_visitado[i] = 0;
    }

    printf("Inicializando as Threads\n");
    // Criando as Threads iniciando a busca em cada um dos processos
    for (int i = 0; i < NUM_PROCESSOS; i++)
    {   
        printf("-#- Criando a Thread do Processo %d na main -#-\n", i);
        no[i] = i;
        pthread_create(&threads[i], NULL, buscar, &no[i]);
    }

    // Dando Join nas Threads e finalizando 
    for (int i = 0; i < NUM_PROCESSOS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("\n");
    printf("-#-#- Finalizando o Programa da QuestÃ£o 2 -#-#-\n");
    printf("\n");
    pthread_exit(NULL);
    return 0;
}
