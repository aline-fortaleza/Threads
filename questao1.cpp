#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cstdio.h>
#include <iostream.h>
#include <ctime.h>
using namespace std;

// Definir quantidade de Threads que vai rodar no programa
#define Quantidade_de_Threads_teste_1 1;
#define Quantidade_de_Threads_teste_2 5;
#define Quantidade_de_Threads_teste_3 10;

// Struct que carrega as informações da Thread
struct threadInfo {
    int thread_id;
    int posicao;
    std::string message;
};

// Função que a Thread tenta achar o caracter correto usando força bruta
void* testarCaracter(void *arg){ 
    
    threadInfo* info = static_cast<threadInfo*>(arg);
    
    int tid = info->thread_id;
    int posicao = info->posicao;
    int decifrar = info->message[posicao];
    int decifrador = 48;
    int error = 0;

    printf("Iniciando a thread %d, de posicao %d, decifrando o caracter %c, decifrador inicia do caracter = %c\n", tid, posicao, decifrar, decifrador);

    // Força bruta para poder encontrar o valor do caracter da posição da senha a ser decifrada
    for(int i = 0; i < 3; i++){

        if(decifrador == decifrar){
            i = 10;
        } else if (decifrador == 122) {
            i = 10;
            error = 1;
        } else {
            decifrador++;
        }

        i--;
    }   

    // Se não encontrar retornar ERROR e se encontrar retornar o valor do caracter 
    if(error == 1){
        printf("ERROR ERRROR ERROR ERROR ERROR ERROR ERROR ERRROR ERROR ERROR ERROR ERROR\n");
        pthread_exit(nullptr);
    } else {
        int *result = new int;
        *result = decifrador;
        printf("Thread %d, Posicao %d, Caracter Decifrado = %c\n", tid, posicao, decifrador);
        pthread_exit(result);
    }

    return nullptr;
}

// int main que roda a base do prorgrama
int main (){  

    printf("\n");
    double testeTime[3];

    // Rodando os 3 testes para comparar os tempos de execução
    for(int teste = 0; teste < 3; teste ++){

        // Iniciar o clock para saber o tempo de execução do programa
        clock_t start_time = clock();

        // Nessa etapa o qtdThreads recebe a quantidade de Threads do teste
        int qtdThreads = 1;
        if(teste == 0){
            qtdThreads = Quantidade_de_Threads_teste_1;
        } else if(teste == 1){
            qtdThreads = Quantidade_de_Threads_teste_2;
        } else {
            qtdThreads = Quantidade_de_Threads_teste_3;
        }


        // Definindo os tamanhos da senha e senhaDecifrada 
        int tamanhoSenha = 10;
        int senhaDecifrada[10];

        // Definindo a senha a ser decifrada
        string senha("testeSenha");

        // Definindo as Threads e a Struct
        pthread_t threads[qtdThreads]; 
        threadInfo thread_info[qtdThreads];

        printf("Quantidade de Threads do Teste = %d // Quantidade de Rodadas Necessarias = %d\n", qtdThreads ,tamanhoSenha/qtdThreads);

        // for que vai rodar para a quantidade de rodadas que for necessario para encontrar a senha
        for(int rodada = 0; rodada < tamanhoSenha/qtdThreads; rodada++ ){

            // Rodar o pacote de Threads para decifrar a parte da senha a depender da quantidade de Threads
            for(int i=0; i<qtdThreads; i++){

                printf("No main: criando thread %d\n", i);

                thread_info[i].thread_id = i;
                thread_info[i].posicao = rodada*qtdThreads + i;
                thread_info[i].message = senha;

                pthread_create( &threads[i] ,   nullptr ,  testarCaracter, &thread_info[i]); 
            }
        
            // Pegar o resultado encontrado e dando Join nas Threads para que possa rodar novamente
            for(int i=0; i<qtdThreads; i++) {
                void* res;
                pthread_join(threads[i], &res);
                int *resultPtr = static_cast<int*>(res);
                senhaDecifrada[thread_info[i].posicao] = *resultPtr;
                delete resultPtr; // Liberar a memória alocada na thread
                printf("Desligando Thread %d\n", i);
            }  

        }   

        // Guardando as informações em uma String
        char resposta[11];

        for(int i =0; i<10; i++){
            resposta[i] = senhaDecifrada[i];
        }
        resposta[10] = '\0';


        // Printando os Resultados
        printf("\n");
        printf("SENHA DECIFRADA == %s\n", resposta);

        // Finalizando o Clock para saber o tempo de execução
        clock_t end_time = clock();
            
        // Printar o tempo de execução
        double execution_time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;

        printf("Tempo de execução do Programa para o teste com %d Threads: %.6f segundos\n", qtdThreads ,execution_time);
        printf("\n");
        testeTime[teste] = execution_time;
        // Fim do Loop do Teste
    }

    // Printar o resultado dos 3 testes
    printf("\n");
    int time1 = Quantidade_de_Threads_teste_1;
    int time2 = Quantidade_de_Threads_teste_2;
    int time3 = Quantidade_de_Threads_teste_3;
    printf("Tempo de execução do Programa para o teste 1 com %d Threads: %.6f segundos\n" ,time1, testeTime[0]);
    printf("Tempo de execução do Programa para o teste 2 com %d Threads: %.6f segundos\n" ,time2, testeTime[1]);
    printf("Tempo de execução do Programa para o teste 3 com %d Threads: %.6f segundos\n" ,time3, testeTime[2]);
    printf("\n");

    // Finalizar tudo
    pthread_exit(NULL);
    return 0;
}



#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
