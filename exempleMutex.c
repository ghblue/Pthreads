#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMTHRDS 4
#define VECLEN 100

 /*   
 A estrutura a seguir contém as informações necessárias
 para permitir que a função "dotprod" acesse seus dados de entrada e
 coloque sua saída na estrutura.
 */

 typedef struct 
  {
    double      *a;
    double      *b;
    double     sum; 
    int     veclen; 
  } DOTDATA;

 /* Definir variáveis ​​globalmente acessíveis e um mutex*/

    DOTDATA dotstr; 
    pthread_t callThd[NUMTHRDS];
    pthread_mutex_t mutexsum;

 /*
 A função dotprod é ativada quando o thread é criado.
 Toda entrada para esta rotina é obtida a partir de uma estrutura
 do tipo DOTDATA e toda a saída desta função é escrita dentro
 dessa estrutura. O benefício desta abordagem é evidente para o
 programa multi-threaded: quando um thread é criado passamos um único
 argumento para a função ativada - normalmente este argumento
 é um número de thread. Todas as outras informações exigidas pelo
 função é acessada a partir da estrutura globalmente acessível.
 */  

 void *dotprod(void *arg)
 {

    /* Definir e usar variáveis ​​locais por conveniência */

    int i, start, end, len ;
    long offset;
    double mysum, *x, *y;
    offset = (long)arg;
     
    len = dotstr.veclen;
    start = offset*len;
    end   = start + len;
    x = dotstr.a;
    y = dotstr.b;

    /*
    Execute o produto escalar e atribua resultado
    para a variável apropriada na estrutura..
    */

    mysum = 0;
    for (i=start; i<end ; i++) 
     {
       mysum +=  (x[i] * y[i]);
     }

    /*
    Bloquear um mutex antes de atualizar o valor no compartilhamento
    estrutura e desbloqueá-lo na atualização.
    */
    pthread_mutex_lock (&mutexsum);
    dotstr.sum += mysum;
    pthread_mutex_unlock (&mutexsum);

    pthread_exit((void*) 0);
 }

 /* 
 O programa principal cria threads que fazem todo o trabalho e depois
 imprimir o resultado após a conclusão. Antes de criar os threads,
 os dados de entrada são criados. Como todos os threads atualizam uma estrutura compartilhada,
 precisamos de um mutex para exclusão mútua. O segmento principal precisa aguardar
 todos os threads para completar, espera por cada um dos threads. Nós especificamos
 um valor de atributo de encadeamento que permite que o encadeamento principal se junte ao
 threads que cria. Note também que liberamos as alças quando estão
 não é mais necessário.
 */

 int main (int argc, char *argv[])
 {
    long i;
    double *a, *b;
    void *status;
    pthread_attr_t attr;  

    /* Atribuir armazenamento e inicializar valores*/
    a = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
    b = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
   
    for (i=0; i<VECLEN*NUMTHRDS; i++)
      {
      a[i]=1.0;
      b[i]=a[i];
      }

    dotstr.veclen = VECLEN; 
    dotstr.a = a; 
    dotstr.b = b; 
    dotstr.sum=0;

    pthread_mutex_init(&mutexsum, NULL);
         
    /* Crie threads para executar o produto pontocom  */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i=0; i<NUMTHRDS; i++)
    {
    /* 
    Cada encadeamento funciona em um conjunto diferente de dados. O deslocamento é especificado
    por 'i'. O tamanho dos dados para cada segmento é indicado por VECLEN.
    */
    pthread_create(&callThd[i], &attr, dotprod, (void *)i);
    }

    pthread_attr_destroy(&attr);

    /* Espere nos outros threads */
    for(i=0; i<NUMTHRDS; i++)
       {
       pthread_join(callThd[i], &status);
       }

    /* Depois de entrar, imprima os resultados e limpeza */
    printf ("Sum =  %.2f \n", dotstr.sum);
    free (a);
    free (b);
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
}