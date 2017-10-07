#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int *a;

typedef struct range_tuple
{
	int start, end;
}range_tuple;

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

void *threaded_merge_sort(void *argument) {
	int i, j, k;
	range_tuple range = *(range_tuple *)argument;
	int left = range.start, mid, right = range.end;
	range_tuple lrange, rrange;
	int n = right-left+1;
	pthread_t lthread, rthread;
	if(left < right) 
	{
		if(n<5)
		{
			selectionSort(a+left,n);
			return NULL;
		}
		mid = (left + right) >> 1;
		lrange.start = left, lrange.end = mid;
		rrange.start = mid + 1, rrange.end = right;
		pthread_create(&lthread, NULL, threaded_merge_sort, (void *)&lrange);
		pthread_create(&rthread, NULL, threaded_merge_sort, (void *)&rrange);
		pthread_join(lthread, NULL);
		pthread_join(rthread, NULL);
		int n1 = mid - left + 1, n2 = right - mid;
		int *aleft = (int *)malloc(sizeof(int) * n1);
		for(i=0; i<n1; i++)
			aleft[i] = a[left+i];
		int *aright = (int *)malloc(sizeof(int) * n2);
		for(i=0; i<n2; i++)
			aright[i] = a[mid+1+i];
		i=0, k=0, j=0;
		while(i<n1 && j<n2)
		{	
			if(aleft[i]>aright[j])
				a[k+left] = aright[j++];
			else
				a[k+left] = aleft[i++];
			k++;
		}
		while(i<n1){
			a[k+left] = aleft[i++];	
			k++;
		}
		while(j<n2){
			a[k+left] = aright[j++];
			k++;
		}
		free(aleft);
		free(aright);
	}
	return NULL;
}

int main() {
	int n, i;
	range_tuple range;
	scanf("%d", &n);
	a = (int*) malloc(sizeof(int)*n);
	for(i=0; i< n; i++)
		scanf("%d", &a[i]);
	range.start = 0, range.end = n-1;
	pthread_t sort_thread;
	pthread_create(&sort_thread, NULL, threaded_merge_sort, (void *)&range);
	pthread_join(sort_thread, NULL);
	for(i = 0; i < n; i++){
		if (i==n-1)
			printf("%d\n", a[i]);
		else
			printf("%d ", a[i]);
	}
	free(a);
	return 0;
}
