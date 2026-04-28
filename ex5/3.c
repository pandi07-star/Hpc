#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
void* greet(void* arg){
	int id=*(int*)arg;
	printf("Welcome Bro from Thread %d\n",id);
	pthread_exit(NULL);
}
int main(){
	pthread_t threads[10];
	int thread_id[10];
	for(int i=0;i<10;i++){
		thread_id[i]=i;
		pthread_create(&threads[i],NULL,greet,&thread_id[i]);
	}
	for(int i=0;i<10;i++){
		pthread_join(threads[i],NULL);
	}
	printf("Welcome Bro from main thread\n");
}
