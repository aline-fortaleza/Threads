#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>

// Definindo valores fixos para cada tipo de escalonamento
#define staticID 1 
#define dynamicID 2
#define guidedID 3


int comeco = 0; //valores ficiticios utilizados para teste
int end = 15; // idem ^
int ans[100];
int step;

#define OMP_NUM_THREADS 4

int qnt_p_thr;

typedef struct { 
    int taskid;
    int *valoresiniciais;
    int sizechunk;
    int qnt_ult_pacote;
    int inicial; //---- para dinamico
    int final; // idem
} threadinfo;

pthread_mutex_t mutexzinho = PTHREAD_MUTEX_INITIALIZER; // mutex para os escalonamentos dinamicos

int *vid; // Valores Iniciais Dinamicos
bool task[OMP_NUM_THREADS];
bool flag; //flag para indicidar se todas tarefas foram concluidas (flag == true -> acabou)

/*
==== f(i) ====
f(i) irá guardar num vetor resposta qual thread acessou essa posição
*/
void *solve(void* threaddata){
    threadinfo aux3 = *((threadinfo*) threaddata);
    
    for(int i = 0; i < qnt_p_thr; i++){
        if(i == qnt_p_thr - 1){
            for(int j = aux3.valoresiniciais[i]; j < aux3.valoresiniciais[i] + aux3.qnt_ult_pacote; j+= step){
                ans[j] = aux3.taskid;
            }
        }
        else{
            for(int j = aux3.valoresiniciais[i]; j < aux3.valoresiniciais[i] + (aux3.sizechunk * step) ; j+= step){
                ans[j] = aux3.taskid;
            }
        }
    }
    
    pthread_exit(NULL);
}

void function(int i, threadinfo a){
    printf("Thread %d na iteração %d\n", a.taskid, i);
}

void *solve2(void* threaddata){
    threadinfo aux4 = *((threadinfo*)threaddata);
    int j = 0;
    while(!flag || task[aux4.taskid]){
        if(task[aux4.taskid]){
            for(int i = aux4.inicial; i <= aux4.final; i += step){
                printf("Entrando na regiao critica\n");
                pthread_mutex_lock(&mutexzinho);
                j++;
                function(j, aux4);
                pthread_mutex_unlock(&mutexzinho);
            }
            task[aux4.taskid] = false;
        }
    }
    
    pthread_exit(NULL);
}



void omp_for(int inicio , int passo , int final , int schedule , int chunk_size , void (*f)(int)){
    int loc = schedule;
    pthread_t fios[OMP_NUM_THREADS];
    threadinfo *threaddata[OMP_NUM_THREADS];
    for(int k = 0; k < OMP_NUM_THREADS; k++){
        threaddata[k] = (threadinfo *) malloc(sizeof(threadinfo));
        threaddata[k]->taskid = k;
    }
    
    // chega aqui
    switch(loc){
        case staticID: //static
            // para saber quantos pacotes cada thread irá ter 
            int aux1;
            aux1 = (end - comeco) / (chunk_size * OMP_NUM_THREADS * step);
            int resto = (end - comeco)%(chunk_size * OMP_NUM_THREADS * step);
            if(resto != 0) aux1++; //aux1 vai ter o valor de quantos pacotes cada thread irá ter
            
            qnt_p_thr = aux1; //valor constante para todas threads
            for(int p = 0; p < OMP_NUM_THREADS; p++){
                threaddata[p]->valoresiniciais = (int*) malloc(qnt_p_thr * sizeof(int));
                if(threaddata[p]->valoresiniciais == NULL){
                    printf("Erro de alocação de memória\n");
                    exit(1);
                }
                threaddata[p]->sizechunk = chunk_size;
            }
           
            int aux2; //quantidade de iteracões no ultimo pacote
            aux2 = resto/(OMP_NUM_THREADS );
            for(int p = 0; p < OMP_NUM_THREADS; p++){
                threaddata[p]->qnt_ult_pacote = aux2;
            }

            
            if(resto%(OMP_NUM_THREADS) != 0){ //caso tenha que nos ultimos pacote alguma thread ter mais interacões que outras, vai atribuindo uma por um até a ultima thread
                for(int p = 0; p < resto%(OMP_NUM_THREADS); p++){
                    threaddata[p]->qnt_ult_pacote++;
                }
            }
            
            //salvando os valores iniciail
            for(int p = 0; p <OMP_NUM_THREADS; p++){
                for(int k = 0; k < qnt_p_thr; k++){
                    if(k == 0) threaddata[0]->valoresiniciais[0] = comeco;
                    else{
                        if(k == qnt_p_thr - 1 && p != 0){ //ultimos pacotes tamanho diferente de chunck_size
                            threaddata[p]->valoresiniciais[k] = threaddata[p-1]->valoresiniciais[k] + threaddata[p-1]->qnt_ult_pacote + step - 1;
                        }
                        else { //qundo cada pacote de intereção tiver o tamando do chunck_size
                        threaddata[p]->valoresiniciais[k] = threaddata[p]->valoresiniciais[k - 1] + (chunk_size * OMP_NUM_THREADS * step);
                        }    
                    }
                    //relembrar o pq dessa linha abaixo
                    if(p+1 < OMP_NUM_THREADS) threaddata[p+1]->valoresiniciais[0] = threaddata[p]->valoresiniciais[0] + (chunk_size * step);
                }
            }
           
            for(int j = 0; j < OMP_NUM_THREADS; j++){
                int rc = pthread_create(&fios[j], NULL, solve, (void *)threaddata[j]); 
                if(rc){
                    printf("Erro\n");
                    exit(-1);
                }
                
            }
            
            
            for(int j = 0; j < OMP_NUM_THREADS; j++){
                free(threaddata[j]); //dando free
            }
    
        break;
       
        case dynamicID:
            flag = false;
            int count = 0;
            int novo_inicio = comeco;
            for(int j = 0; j < OMP_NUM_THREADS; j++){
                int rc = pthread_create(&fios[j], NULL, solve2, (void *)threaddata[j]); 
                if(rc){ //checando se foi criada corretamente
                    printf("Erro\n");
                    exit(-1);
                }
                task[j] = false;
            }
            
            while(!flag){
                if(!task[count]){
                    int temp = novo_inicio;
                    novo_inicio += chunk_size * passo;
                    printf("%d\n", novo_inicio);
                    if(novo_inicio >= end){
                        novo_inicio = end - 1;
                        flag = true;
                    } 
                    task[count] = true;
                    threaddata[count]->inicial = temp;
                    threaddata[count]->final = novo_inicio;
                }
                
                count = (count + 1) % OMP_NUM_THREADS;
            }
            for(int j = 0; j < OMP_NUM_THREADS; j++){ //dando join em todas as threads
                pthread_join(fios[j], NULL);
            }
        break;
        
        case 3:
            flag = false;
            int count2 = 0;
            int novo_inicio2 = comeco;
            for(int j = 0; j < OMP_NUM_THREADS; j++){
                int rc = pthread_create(&fios[j], NULL, solve2, (void *)threaddata[j]); 
                if(rc){ //checando se foi criada corretamente
                    printf("Erro\n");
                    exit(-1);
                }
                task[j] = false;
            }
            
            while(!flag){
                if(!task[count]){
                    int temp = novo_inicio;
                    novo_inicio2 += chunk_size * passo;
                    printf("%d\n", novo_inicio);
                    if(novo_inicio >= end){
                        novo_inicio = end - 1;
                        flag = true;
                    } 
                    task[count2] = true;
                    threaddata[count2]->inicial = temp;
                    threaddata[count2]->final = novo_inicio;
                }
                
                count2 = (count2 + 1) % OMP_NUM_THREADS;
            }
            for(int j = 0; j < OMP_NUM_THREADS; j++){ //dando join em todas as threads
                pthread_join(fios[j], NULL);
            }
        break;
    }
    
    for(int j = 0; j < OMP_NUM_THREADS; j++){ //dando join em todas as threads
        pthread_join(fios[j], NULL);
    }
}


int menu(); //funcão criada para imprimir o menu inicial 

int main(){
    int type = 0;
    int chunksize;
    printf("\n- Qual o tamanho dos chunks desejado:\n");
    scanf("%d", &chunksize);
    printf("\n- Qual o passo?\n");
    scanf("%d", &step);
    while(type != 4){
        for(int i = 0; i < end; i++){
            ans[i] = -1;
        }
        
        type = menu();
        switch(type){
            case staticID:
                printf("\n\n--- Método static escolhido ---\n\n");
                omp_for(comeco, step , end, type, chunksize , (void*)solve);
                break;
            case dynamicID:
                printf("\n\n--- Método dynamic escolhido ---\n\n");
                omp_for(comeco, step, end, type, chunksize, (void*) solve2);
                break;
            case guidedID:
                printf("--- Método guided escolhido ---\n\n");
                omp_for(comeco, step, end, type, chunksize,  (void*) solve2);
                break;
            case 4: 
                printf("Finalizando o código\n\n");
                break;
        }
        
        if(type == 1){ //imprimindo o vetor resposta da funcao 1
        printf("\n");
        for(int i = 0; i < 100; i++){
            if(i%40 == 0 && i != 0) printf("\n");
            printf("%d ", ans[i]);
        }
        printf("\n");
        }
    
    }
}

int menu(){
    int escolha; //variavel que sera armazenada o tipo de escalonamento
    
    //printf("\nQuantas vezes o loop será efetuado?\n");
    //scanf("%d", &end);
    
    printf("\n---- Escolha o método de escalonamento ----\n");
    printf("  1. static \n");
    printf("  2. dynamic \n");
    printf("  3. guided \n");
    printf("  4. Encerre o código \n");
    printf("\n");

    scanf("%d", &escolha);

    return escolha;
}
