#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
int last_number = 1; // variavel global de ultimo número primo acessado
int T,N;
bool *numeros;

void *Primos(void *tid){
    int Tid = *((int*)tid );
    printf("Thread %d dentro da função \n", Tid); // saber qual thread entrou na função
    pthread_mutex_lock(&mymutex); // dando lock no mutex para duas threads não mudarem o mesmo valor ao mesmo tempo
    for (int a = last_number + 1; a< N; a++){ // loop que acessará todos os números a partir do último primo
        if (numeros[a] == 1){ // checa se o número ainda está no array
            for (int d = 2; d < (N/a); d++ ){
                numeros[a * d] = 0; // retira todos os multiplos de a até N
                printf ("O número %d foi apagado \n", (a * d));
            }
            last_number = a; // atualiza quem foi o último número primo
            pthread_mutex_unlock(&mymutex);
            pthread_exit(NULL);
        }

    }

    pthread_mutex_unlock(&mymutex);
    pthread_exit(NULL);

}

// int main que roda a base do programa 
int main(int argc, char *argv[]){
    
    // Recebendo do usuario as informações da questão
    
    printf("Digite o número T de Threads: \n");
    scanf(" %d", &T);
    printf("Digite o número N a qual encontraremos todos os números primos até ele:\n");
    scanf(" %d", &N);

    //criando array de bool e setando todos como 1 a partir do 2
    numeros = (bool*) malloc( N* sizeof(bool)); // alocando o vetor 
    numeros[0] = 0;
    numeros[1] = 0;

    for(int i = 2; i < N; i++){
        numeros[i] = 1;
    }

    // definindo as threads e suas ids
    pthread_t threads[T];
    int *taskids[T];
    
    // criando e rodando as treads
    for(int t=0; t< T ; t++){
        printf("Na main: criando thread %d\n", t);    
        taskids[t] = (int *) malloc(sizeof(int)); 
        *taskids[t] = t;
	 	int rc = pthread_create(&threads[t],NULL,Primos, (void *)taskids[t]); 
        if (rc){
            printf("Erro %d de criação da thread", rc);
        }        
    } 

    printf("\n");

    //Liberando as threads
    for (int k=0; k< T; k++) pthread_join(threads[k], NULL);

    // Imprimindo os números primos
    printf ("Os números primos até %d são: \n", N);
    for (int j=0; j < N; j++) {
        if (numeros[j] == 1){
            printf("%d \n", j);
        }
        

    }

    free (numeros); // liberando vetor
    pthread_exit(NULL);

}
