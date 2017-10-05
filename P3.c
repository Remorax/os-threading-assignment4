#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>

void merge(int a[], int l, int m, int r)
{
	int size = (r-l+1);
	int temp[size];
	int i=l, j=m+1, c=0;
	while((i<=m)&&(j<=r))
	{
		if (a[j]<a[i])
			temp[c++] = a[j++];
		else if (a[j]>a[i])
			temp[c++] = a[i++];
		else
		{
			temp[c++] = a[j++];
			temp[c++] = a[i++];
		}
	}
	while(i<=m)
		temp[c++] = a[i++];
	while(j<=r)
		temp[c++] = a[j++];
	for (i=0; i<size; i++,l++)
		a[l]=temp[i];
}

void selectionSort(int a[], int n)
{
	int i,j,key;
	for (i=0; i<(n-1); i++)
	{
		key=i;
		for (j=i+1; j<n; j++)
		{
			if (a[j]<a[key])
				key = j;	
		}
		int temp;
		temp = a[key];
		a[key] = a[i];
		a[i] = temp;
	}
	return;
}

void merge_sort(int sharedArray[], int l, int r)
{
	int len = (r-l+1);
	int mid = (l+(len/2)-1);
	if(len<5)
	{
		selectionSort(sharedArray+l, len);
		return;
	}
	pid_t lpid, rpid;
	lpid = fork();
	if (lpid==0){
		merge_sort(sharedArray, l, mid);
		_exit(0);
	}
	else if (lpid>0){
		rpid = fork();
		if(rpid==0){
			merge_sort(sharedArray, mid+1, r);
			_exit(0);	
		}
		else if(rpid<0){
			perror("Error while forking right child");
			_exit(-1);
		}
	}
	else{
		perror("Error while forking left child");
		_exit(-1);
	}
	int stat;
	waitpid(lpid, &stat, 0);
	waitpid(rpid, &stat, 0);
	merge(sharedArray, l, mid, r);
	return;
}

int main()
{
	int i,n,shmid;
	int* sharedArray;
	key_t key = IPC_PRIVATE;
	printf("Enter number of elements in the array.\n");
	scanf("%d",&n);
	shmid = shmget(key, sizeof(int)*n, IPC_CREAT| 0666);
	
	if(shmid<0){
		perror("Error while creating the segment.");
		exit(1);
	}

	sharedArray = shmat(shmid, 0, 0);
	if (sharedArray == (void*)-1)
	{
		perror("Error attaching the segment");
		exit(1);
	}

	printf("Enter the elements of the array.\n");
	for(i=0;i<n;i++)
		scanf("%d",&sharedArray[i]);

	merge_sort(sharedArray, 0, n-1);
	for(i=0;i<n;i++)
		printf("%d ",sharedArray[i]);
	printf("\n");
	if (shmdt(sharedArray) == -1)
    {
        perror("Error detaching the segment");
        _exit(1);
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("Error deleting the memory segment");
        _exit(1);
    }
	return 0;
}