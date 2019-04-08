#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *OiMundo (void* str){
	printf((char*) str);
	printf("Eu sou a thread %d!!!\n", (int)pthread_self());
}

void main(){
	pthread_t thid;
	char* str = "Oi mundo!!!!";

	if (pthread_create( &thid,NULL,OiMundo,NULL)!=0){
		printf("Ocorreu um erro!!!\n");
		exit(0);
	}

	printf("Foi criada a thread %d.\n", (int)thid);
	pthread_join(thid, NULL);
	printf(" A thread %d já terminou.\n", (int)thid);
}


