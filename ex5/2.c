#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define MAX 100
int arr[MAX];
int n;
int sum1=0,sum2=0;
void* partial_sum1(void* arg){
	for(int i=0;i<n/2;i++){
		sum1+=arr[i];
	}
        printf("Sum 1 is:%d\n",sum1);
	pthread_exit(NULL);
	//printf("Sum 1 is:%d\n",sum1);

}
void* partial_sum2(void* arg){
	for(int i=n/2;i<n;i++){
		sum2+=arr[i];
	}
        printf("Sum 2 is:%d\n",sum2);
	pthread_exit(NULL);
        //printf("Sum 2 is:%d\n",sum2);

}
int main(){
	pthread_t t1,t2;
	printf("Enter the number of elements:");
	scanf("%d",&n);
	printf("Enter %d numbers :\n",n);
	for(int i=0;i<n;i++){
		scanf("%d",&arr[i]);
	}
	pthread_create(&t1,NULL,partial_sum1,NULL);
	pthread_create(&t2,NULL,partial_sum2,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	int total=sum1+sum2;
	printf("Total Sum is:%d\n",total);
}
