#include <stdio.h>
#include <malloc.h>
#include <pthread.h>

struct descritor {
  int vet[100];
  int size;
  int i, e;
  pthread_mutex_t m;
};

void* soma( void* dta ) {
  int *aux, i;
  struct descritor *d = (struct descritor*) dta;
  printf("Soma\n");
  aux = (int*) malloc(sizeof(int));
  *aux = 0;

  for( i = d->i ; i < d->e ; ++i )
    *aux += d->vet[i];

  return aux;
}

int main() {
  pthread_t id;
  struct descritor desc;
  int *res, i;

  for( i = 0 ; i < 100 ; ++i ) desc.vet[i] = i;

  desc.size = 100;
  desc.i = 0;
  desc.e = 100;

  pthread_create(&id,NULL, soma,(void*)&desc);
  pthread_join(id,(void*)&res);
  printf("res = %d\n",*res);
  free(res);

  return 0;
}


