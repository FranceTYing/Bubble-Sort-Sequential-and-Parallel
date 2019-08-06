#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>

__global__ void even(int *darr, int n)
{
	int k = threadIdx.x + blockIdx.x*blockDim.x;
	int t;
	k= k * 2;
	if (k<=n-2)
	{
		if (darr[k] >darr[k+1])
		{
			t=darr[k];
			darr[k]=darr[k+1];
			darr[k+1]=t;
		}
	
	}
	
}

__global__ void odd(int *darr, int n)
{
	int k = threadIdx.x + blockIdx.x*blockDim.x ;
	int t;
	k= k*2+1;
	if (k<=n-2)
	{
		if (darr[k] >darr[k+1])
		{
			t=darr[k];
			darr[k]=darr[k+1];
			darr[k+1]=t;
		}
	
	}
	
}

void print(int arr[], int row, double t)
{
	int j;
	char filename[32];
	//append file name
	snprintf(filename, sizeof(char) * 32, "OUTPUT_%d.dat", row);
	
	FILE * f = fopen(filename, "w");
	for(j = 0; j < row; j++)
	{
		fprintf(f, "%d\n", arr[j]);
	}
	
	fprintf(f, "\nTime taken = %f seconds. \n", t);
	fclose(f);
}

int main()
{
	int row = 0;
	int temp = 0;
	FILE *file = fopen("data500k.txt", "r");
	int *input = NULL;
	char random;
	double s, e, timeTaken;
	struct timeval t1, t2;
	
	
	if(file == NULL)
	{
		printf("Unable to open file! ");
		return 1;
	}
	else
	{
		printf("Opening file......");
		while(fscanf(file, "%d", &temp) != EOF)
		{
			if(input == NULL)
			{
				input = (int*)malloc(sizeof(temp));
				*input = temp;
			}
			else
			{
				input = (int*)realloc(input, (row+1)*sizeof(temp));
				input[row] = temp;
			}
			row++;
		}
		
		printf("Done read file. ");
		
	}
	fclose(file);
	printf ("\n");
	printf("Press any key to continue......\n");
	random = getchar();
	
	int *copy = (int*)malloc(row * sizeof(int));
	int Line=50000;
	int copyRow;
	while (Line <=row){
	for(copyRow = 0; copyRow < Line; copyRow++)
	{
		copy[copyRow] = input[copyRow];
	}

	int *arr, *darr;
	arr = (int*)malloc (row* sizeof(int));
	int i,copyrow;
	for (copyrow=0; copyrow<copyRow; copyrow++)
	{
		arr[copyrow] =  copy[copyrow];
	}
	
	cudaMalloc(&darr, copyrow*sizeof(int));
	cudaMemcpy(darr, arr, copyrow*sizeof(int), cudaMemcpyHostToDevice);
	//start clock
	
	gettimeofday(&t1, NULL);
		s = (double)(t1.tv_sec + (t1.tv_usec / 1.0e9));

	
	for (i=0; i<=copyrow/2; i++)
	{
		even<< <copyrow/1000, 1000>> >(darr, copyrow);
		odd<< <copyrow/1000, 1000>> >(darr, copyrow);
	}
	gettimeofday(&t2, NULL);
		e = (double)(t2.tv_sec + (t2.tv_usec / 1.0e9));

	timeTaken = e - s;
	cudaMemcpy(arr, darr, copyrow*sizeof(int), cudaMemcpyDeviceToHost);
	
	printf("\nWhen number of line = %d, ", copyrow);
	printf("Time taken to sort the elements = %f seconds. \n", timeTaken);
	print(arr, copyrow, timeTaken);
	Line= Line + 50000;
	}
	
	getchar();
	return 0;
}