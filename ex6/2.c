#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int available_seats = 50;
int booking_requested = 0;
int done = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* user_thread(void* arg){
	int seats;
	while(1){
		printf("Enter seats to book: ");
		scanf("%d", &seats);
		if(seats==0){
			pthread_mutex_lock(&mutex);
			done = 1;
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_lock(&mutex);
		booking_requested = seats;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* booking_thread(void* arg){
	while(1){
		pthread_mutex_lock(&mutex);
		while(booking_requested==0 && !done)
			pthread_cond_wait(&cond, &mutex);
		if(done){
			pthread_mutex_unlock(&mutex);
			break;
		}

		if(booking_requested <= available_seats){
			available_seats -= booking_requested;
			printf("Booking confirmed! %d seats booked. %d seats left.\n", booking_requested, available_seats);
		}
		else{
			printf("Sorry not enough seats\n");
		}
		booking_requested = 0;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* display_thread(void* arg){
	while(!done){
		printf("Available seats: %d\n", available_seats);
		sleep(1);
	}
	printf("Final seats remaining: %d\n", available_seats);
	return NULL;
}

int main(){
	pthread_t user, booking, display;
	pthread_create(&display, NULL, display_thread, NULL);
	pthread_create(&booking, NULL, booking_thread, NULL);
	pthread_create(&user, NULL, user_thread, NULL);
	pthread_join(user, NULL);
	pthread_join(booking, NULL);
	pthread_join(display, NULL);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}
