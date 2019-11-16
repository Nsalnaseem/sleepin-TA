#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <strings.h>
#include <unistd.h>

void *TA_routine();
void *student_routine(void * studentNum);
void printArr(int arr[]);

int waitingChairs[3], waitingCount, inDesk, helping;
pthread_mutex_t desk, chairs;
sem_t sleeping;

int main(int argc, char *argv[]){

	if (argc !=2){
		printf("Invalid Entry You must enter student number in commaned line argument.\n");
		exit(1);
	}

	int n=atoi(argv[1]);
	if(n<0){
		printf("Number of student must be >= 0\n");
		exit(1);	
	}
	
	pthread_t ta, students[n];
	long i;
	helping=n;
	
	//bzero(waitingChairs, sizeof(waitingChairs));
	for(i=0;i<3;i++)
		waitingChairs[i]=-1;
	waitingCount=0;
	inDesk=-1;
	pthread_mutex_init(&desk, NULL);
	pthread_mutex_init(&chairs, NULL);
	sem_init(&sleeping, 0, 0);

	
	
	//creating threads
	pthread_create(&ta, NULL, TA_routine, NULL);
	for(i=0; i<n; i++)
		pthread_create(&students[i], NULL, student_routine,(void *)i);
	//joining threads	
	for(i=0; i<n; i++)
		pthread_join(students[i], NULL);
	pthread_join(ta,NULL);
	printf("TA is Finshed helping all %d Students\n",n);
	return 0;

}


void *student_routine(void * studentNum){
	int i, num= (long) studentNum, chairFound=0;

	printf("Student #%d is programming.\n",num);
	printf("Student #%d needs TA help.\n",num);		
	while(1){
		
		if( (inDesk==-1) && (waitingCount==0) ){
			pthread_mutex_lock(&desk);
			inDesk=num;
			sem_post(&sleeping);
		
			pthread_mutex_unlock(&desk);	
			break;
		}else if(waitingCount<3){
			pthread_mutex_lock(&chairs);
			
			for(i=0 ;i<3 ;i++){
				if(waitingChairs[i]==-1){
					waitingChairs[i]=num;
					waitingCount++;
					chairFound=1;
					break;
				}
			}//check for empty chair
			pthread_mutex_unlock(&chairs);
			if(chairFound==1){
				printf("Student #%d is sitting on waiting chair [%d]\n",num,i);	
				break;
				
			}
		}else {
				printf("Student #%d Couldn't find a chair ",num);
				printf("continued programming insted.\n");
				sleep(5);//continue programming for 5 sec

		}
		

	}
}

void *TA_routine(){
	int i;
	while(1){
		if(helping ==0)
			break;
		sem_wait(&sleeping);
		printf("ZZZZZZZZZZZZZ [TA Woke up] ZZZZZZZZZZZZZZZZZ\n");
		printf("TA is now helping student #%d\n",inDesk);
		sleep(2);///////////////////////
		helping --;
		if(waitingCount!=0){
			for(i=0;i<3;i++){
				if(waitingChairs[i]!=-1){
					inDesk=waitingChairs[i];
					printf("TA is now helping waiting student #%d\n",inDesk);
					sleep(2);
					waitingChairs[i]=-1;
					waitingCount--;	
					helping --;
				}				
			}	
		}
		inDesk=-1;
		
		printf("ZZZZZZZZZZZZZ [TA is back to sleep] ZZZZZZZZZZZZZZZZZ\n");
	}


}

void printArr(int arr[]){
	int i;
	printf("Waiting:[ ");
	for(i=0;i<3;i++)
		printf("%d",arr[i]);
	printf("]\n");
}


