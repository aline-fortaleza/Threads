#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


//Formato de Entrada:
/*
    x=numero de linhas
    y=numero de colunas
    n=numero de threads
    
    x y n
    (matrix)
    
    exemplo:
    4 4 2
    1 0 0 0
    0 1 1 0
    1 0 0 0
    1 0 0 0
    
    A forma de divisÃ£o de trabalho entre theads foi escolhina de tal forma:
    Se o mapa tem tamanho x e y, x serÃ¡ dividido pelo nÃºmero de theads n de tal forma
    que cada thead cuide  de aproximadamente y*(x/n) itens da matriz, e ao juntar todas
    as theads, teremos a quantidade total x*y de itens
    

*/









struct entrada{
    int xMin;
    int xMax;
    int Ytemp;
};


struct entrada *saux;
int **m1;
int **m2;
int num_ilhas=0;
int auxMesmaIlha=1;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

void MesmaIlha(int xMin,int xFin,int yMin,int yFin,int x,int y)
{
    //Backtracking em que x varia entre xMin e xMax, y entre yMin e yMax, x e y representam o lugar da matriz
    if(x<xMin || y<yMin || x>=xFin || y>=yFin || m1[x][y]!=1)
        return;
    
    m1[x][y]=2;
    m2[x][y]=auxMesmaIlha; //modificando a matriz auxiliar que serÃ¡ necessaria para juntar o mapa
    MesmaIlha(xMin,xFin,yMin,yFin,x+1,y);
    MesmaIlha(xMin,xFin,yMin,yFin,x,y+1);
    MesmaIlha(xMin,xFin,yMin,yFin,x-1,y);
    MesmaIlha(xMin,xFin,yMin,yFin,x,y-1);
    MesmaIlha(xMin,xFin,yMin,yFin,x+1,y+1);
    MesmaIlha(xMin,xFin,yMin,yFin,x-1,y+1);
    MesmaIlha(xMin,xFin,yMin,yFin,x-1,y-1);
    MesmaIlha(xMin,xFin,yMin,yFin,x+1,y-1);
}

void MesmaIlha2(int xMin,int xFin,int yMin,int yFin,int x,int y,int num1,int num2)
{
    //Backtrackin auxiliar, apenas para resolver as regiÃµes de fronteira das threads
    if(x<xMin || y<yMin || x>=xFin || y>=yFin || m2[x][y]!=num2)
        return;
    
    m2[x][y]=num1;
    MesmaIlha2(xMin,xFin,yMin,yFin,x+1,y,num1,num2);
    MesmaIlha2(xMin,xFin,yMin,yFin,x,y+1,num1,num2);
    MesmaIlha2(xMin,xFin,yMin,yFin,x-1,y,num1,num2);
    MesmaIlha2(xMin,xFin,yMin,yFin,x,y-1,num1,num2);
    MesmaIlha2(xMin,xFin,yMin,yFin,x+1,y+1,num1,num2);
    MesmaIlha2(xMin,xFin,yMin,yFin,x-1,y+1,num1,num2);
    MesmaIlha2(xMin,xFin,yMin,yFin,x-1,y-1,num1,num2);
    MesmaIlha2(xMin,xFin,yMin,yFin,x+1,y-1,num1,num2);
}






void* Partes(void *ptr)                         //Processamento de cada thread
{
    struct entrada temp=*((struct entrada*)ptr);
    int xMin=temp.xMin;
    int xMax=temp.xMax;
    int Ytemp=temp.Ytemp;
    
    
    for(int i=xMin;i<xMax;i++)
    {
        for(int j=0;j<Ytemp;j++)
        {
            if(m1[i][j]==1)
            {
                pthread_mutex_lock(&mymutex);       //mutex necessÃ¡rio devido a condiÃ§Ã£o de corrida 
                num_ilhas = num_ilhas + 1; 
                MesmaIlha(xMin,xMax,0,Ytemp,i,j);   //Backtracking
                auxMesmaIlha++;                     //Linha que gera condiÃ§Ã£o de corrida, cada ilha deve ter um valor diferente
                pthread_mutex_unlock(&mymutex);
            }
        }
    }
    pthread_exit(NULL);
}

void definirEntrada(int n, int x,int y)//Aqui serÃ¡ definida a area dde atuaÃ§Ã£o de cada thread
{
    int z=x;
    int m=x/n;
    int i=0;
    for(i=0;i<n-1;i++)
    {
        saux[i].xMin=m*i;
        saux[i].xMax=m*(i+1);
        saux[i].Ytemp=y;
        z=z-m;
    }
    saux[i].xMin=m*i;
    saux[i].xMax=(m*i)+z;
    saux[i].Ytemp=y;
    return;
    //xMin e Xmax irÃ£o variar a cada theads
    //o valor de y sempre terÃ¡ o mesmo alcanÃ§e
}

int main()
{
    int n,x,y;
    scanf("%d",&x);
    scanf("%d",&y);
    scanf("%d",&n);
    struct entrada Entrada[n];
    pthread_t thread[n];
    saux=(struct entrada*)malloc(sizeof(struct entrada)*(n));   //vetor auxiliar
    m1=(int **) malloc(sizeof(int*)*x);                         //Mapa com as ilhas
    m2=(int **) malloc(sizeof(int*)*x);                         //Mapa auxiliar
    definirEntrada(n,x,y);                                      //Aqui serÃ¡ realizada a divisÃ£o de trabalho das theads
    for(int i=0;i<n;i++)
    {
        Entrada[i].xMin=saux[i].xMin;
        Entrada[i].xMax=saux[i].xMax;
        Entrada[i].Ytemp=saux[i].Ytemp;
        //printf("%d %d %d\n",Entrada[i].xMin,Entrada[i].xMax,Entrada[i].Ytemp);
        //Inserindo os valores que serÃ£o utilizados por cada thead
    }
    for(int i=0;i<x;i++)
    {
        m1[i]=(int *) malloc(sizeof(int)*y);
        m2[i]=(int *) malloc(sizeof(int)*y);
        for(int j=0;j<y;j++)
        {
            scanf("%d",&m1[i][j]); //obtendo o mapa com as ilhaas
        }
    }
    
    for(int i=0;i<n;i++)
    {
        int ret = pthread_create( &thread[i], NULL, Partes, (void*)&Entrada[i]); //criaÃ§Ã£o  das theads, o vetor Entrada determina em que parte do mapa elas irÃ£o agir
    }
    for(int i=0;i<n;i++)
    {
        pthread_join(thread[i], NULL); //sÃ³ continuar o codigo apÃ³s as theads serem executadas
    }

    
    for(int i=0;i<x;i++) //Nessa parte do cÃ³digo sÃ£o resolvidas as regiÃµes de fronteira no mapa(entre duas theads)
    {
        for(int j=0;j<y;j++)
        {
            if(m2[i][j]!=0)
            {
                int h; int r;
                if(i+1<x && m2[i][j]!=m2[i+1][j] && m2[i+1][j]!=0)
                {
                    h=m2[i][j];
                    r=m2[i+1][j];
                    num_ilhas=num_ilhas-1;
                    MesmaIlha2(0,x,0,y,i+1,j,m2[i][j],m2[i+1][j]);
                }
                else if(j+1<y && m2[i][j]!=m2[i][j+1] && m2[i][j+1]!=0)
                {
                    num_ilhas=num_ilhas-1;
                    MesmaIlha2(0,x,0,y,i,j+1,m2[i][j],m2[i][j+1]);
                }
                else if(i+1<x && j+1<y && m2[i][j]!=m2[i+1][j+1] && m2[i+1][j+1]!=0)
                {
                    num_ilhas=num_ilhas-1;
                    MesmaIlha2(0,x,0,y,i+1,j+1,m2[i][j],m2[i+1][j+1]);
                }
                else if(i+1<x && j-1>=0 && m2[i][j]!=m2[i+1][j-1] && m2[i+1][j-1]!=0)
                {
                    num_ilhas=num_ilhas-1;
                    MesmaIlha2(0,x,0,y,i+1,j-1,m2[i][j],m2[i+1][j-1]);
                }
            }
            
        }
    }
    
    printf("%d\n",num_ilhas); //printar o numero de ilhas
    
    
}
