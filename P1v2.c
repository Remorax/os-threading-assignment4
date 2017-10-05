#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#define mysleep(s) usleep(s*1000000)

pthread_mutex_t mutex, mutex2, mutex3;
pthread_cond_t cond, cond2, cond3, cond4, cond5, cond6, cond7, cond8, cond9, cond10;
int currentNumber, currentCar, allCarsServed = 0, ongoing=-1, typeOfService=-1;
int gasStationArray[3];

void pay(int id, int gasStationNo)
{
	printf("Car %d is paying\n",id);
	typeOfService = 1;
	if (gasStationNo==1)
		pthread_cond_signal(&cond3);
	else if (gasStationNo==2)
		pthread_cond_signal(&cond4);
	else
		pthread_cond_signal(&cond5);
}

int goToPump(int id, int gasStationNo)
{
	pthread_mutex_lock(&mutex);
	printf("Car %d was assigned to gas station %d\n",id,gasStationNo);
	currentCar = id;
	if(ongoing<0)
		ongoing = gasStationNo;
	else
		pthread_cond_wait(&cond6, &mutex);
	typeOfService = 0;
	if (gasStationNo==1)
		pthread_cond_signal(&cond3);
	else if (gasStationNo==2)
		pthread_cond_signal(&cond4);
	else
		pthread_cond_signal(&cond5);
	pthread_mutex_lock(&mutex2);
	pthread_cond_wait(&cond6, &mutex2);
	pthread_mutex_unlock(&mutex2);
	pay(id,gasStationNo);
	pthread_mutex_lock(&mutex3);
	pthread_cond_wait(&cond7, &mutex3);
	pthread_mutex_unlock(&mutex3);
	printf("Car %d's work is now done and it exits gas station %d.\n",id,gasStationNo);
	pthread_mutex_unlock(&mutex);
	pthread_mutex_lock(&mutex);
	gasStationArray[gasStationNo-1] = -1;
	currentNumber--;
	pthread_mutex_unlock(&mutex);
	return gasStationNo;
}

bool pickPump(int idx)
{
	if(gasStationArray[0]==-1){
		gasStationArray[0] = idx;
		pthread_mutex_unlock(&mutex);
		goToPump(idx,1);
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
		return true;
	}
	else if (gasStationArray[1]==-1)
	{
		gasStationArray[1] = idx;
		pthread_mutex_unlock(&mutex);
		goToPump(idx,2);
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
		return true;
	}
	else if (gasStationArray[2]==-1)
	{
		gasStationArray[2] = idx;
		pthread_mutex_unlock(&mutex);
		goToPump(idx,3);
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
		return true;
	}
	return false;
}

void waitInLine(int idx)
{
	printf("Car %d is waiting in a queue\n", idx);
	int x = pthread_cond_wait(&cond, &mutex);
	if(x==0)
		printf("Vacancy at a gas pump. So car %d picks one of the empty gas pumps.\n",idx);
	else
		fprintf(stderr,"An error occured while waiting: %s\n", strerror(x));
	pthread_cond_signal(&cond2);
	pickPump(idx);
}

void* enterStation(void* index)
{
	pthread_mutex_lock(&mutex);
	int *idx = (int*) index;
	(*idx)++;
	if(!pickPump(*idx))
		waitInLine(*idx);
	return (void*)1;
}

void serveCar(int attender)
{
	printf("Car %d is being served by Attender %d\n",currentCar, attender);
	sleep(1);
	return;
}

void acceptPayment(int attender){
	printf("Payment from car %d is being accepted by attender %d\n",currentCar, attender);
	mysleep(0.5);
	return;	
}

void* idleAttender1()
{
	printf("Attender 1 has been created and is dreaming about having his own car\n");
	while(allCarsServed==0)
	{
		if(typeOfService==0){
			pthread_mutex_lock(&mutex2);
			pthread_cond_wait(&cond3, &mutex2);
			serveCar(1);
			ongoing = -1;
			pthread_cond_broadcast(&cond6);
			printf("Attender 1 has finished his service and is dreaming again\n");	
			pthread_mutex_unlock(&mutex2);
		}
		else if (typeOfService==1){
			pthread_mutex_lock(&mutex3);
			pthread_cond_wait(&cond7, &mutex3);
			acceptPayment(1);
			pthread_cond_broadcast(&cond7);
			printf("Attender 1 has finished his service and is dreaming again\n");	
			pthread_mutex_unlock(&mutex3);	
		}
	}
	printf("Exiting1....\n");
	return (void*)1;
}

void* idleAttender2()
{
	printf("Attender 2 has been created and is dreaming about having his own car\n");
	while(allCarsServed==0)
	{
		if(typeOfService==0){
			pthread_mutex_lock(&mutex2);
			pthread_cond_wait(&cond4, &mutex2);
			serveCar(2);
			ongoing = -1;
			pthread_cond_broadcast(&cond6);
			printf("Attender 2 has finished his service and is dreaming again\n");	
			pthread_mutex_unlock(&mutex2);
		}
		else if (typeOfService==1){
			pthread_mutex_lock(&mutex3);
			pthread_cond_wait(&cond8, &mutex3);
			acceptPayment(2);
			pthread_cond_broadcast(&cond8);
			printf("Attender 2 has finished his service and is dreaming again\n");
			pthread_mutex_unlock(&mutex3);		
		}
	}
	printf("Exiting2....\n");
	return (void*)1;
}

void* idleAttender3()
{
	printf("Attender 3 has been created and is dreaming about having his own car\n");
	while(allCarsServed==0)
	{
		if(typeOfService==0){
			pthread_mutex_lock(&mutex2);
			pthread_cond_wait(&cond5, &mutex2);
			serveCar(3);
			ongoing = -1;
			pthread_cond_broadcast(&cond6);
			printf("Attender 3 has finished his service and is dreaming again\n");	
			pthread_mutex_unlock(&mutex2);
		}
		else if (typeOfService==1){
			pthread_mutex_lock(&mutex3);
			pthread_cond_wait(&cond5, &mutex3);
			acceptPayment(3);
			pthread_cond_broadcast(&cond7);
			printf("Attender 3 has finished his service and is dreaming again\n");		
			pthread_mutex_unlock(&mutex3);
		}
	}
	printf("Exiting3....\n");
	return (void*)1;
}

int main()
{
	int numberOfThreads,i;
	for (i=0; i<3; ++i)
		gasStationArray[i] = -1;
	printf("Enter the number of cars:\n");
	scanf("%d",&numberOfThreads);
	srand((unsigned int)time(NULL));
	int r;
	pthread_t cars[numberOfThreads];
	pthread_t attender1, attender2, attender3;
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex2, NULL);
	pthread_mutex_init(&mutex3, NULL);

    pthread_cond_init(&cond, NULL);
    pthread_cond_init(&cond2, NULL);
    pthread_cond_init(&cond3, NULL);
    pthread_cond_init(&cond4, NULL);
    pthread_cond_init(&cond5, NULL);
    pthread_cond_init(&cond6, NULL);
    pthread_cond_init(&cond7, NULL);
    pthread_cond_init(&cond8, NULL);
    pthread_cond_init(&cond9, NULL);
    pthread_cond_init(&cond10, NULL);

	int indexes[numberOfThreads];

	pthread_create(&attender1, NULL, idleAttender1, NULL);
	pthread_create(&attender2, NULL, idleAttender2, NULL);
	pthread_create(&attender3, NULL, idleAttender3, NULL);
	
	for (i = 0; i < numberOfThreads; ++i)
	{
		indexes[i] = i;
		if (currentNumber<7)
		{
			printf("Car %d enters the gas station successfully.\n",i+1);
			pthread_create(&cars[i], NULL, enterStation, &indexes[i]);
			r=rand()%3;
			sleep(r);
		}
		else
		{
			printf("The gas station is full to its capacity. So car %d waiting for a car to get fully serviced\n",i+1);
			currentNumber++;
			pthread_cond_wait(&cond2, &mutex);
			waitInLine(i+1);
		}	
	}

	for (i = 0; i < numberOfThreads; ++i)
		pthread_join(cars[i], NULL);
	pthread_mutex_lock(&mutex);
	allCarsServed = 1;
	pthread_mutex_unlock(&mutex);
	return 0;
}

	