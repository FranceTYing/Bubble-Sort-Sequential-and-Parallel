#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

void print(int arr[], int row, double t);
void bubbleSort(int arr[], int n);

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
				input = malloc(sizeof(temp));
				*input = temp;
			}
			else
			{
				input = realloc(input, (row+1)*sizeof(temp));
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
	
	int *copy = malloc(row * sizeof(int));
	int copyRow;
	for(copyRow = 0; copyRow < row; copyRow++)
	{
		copy[copyRow] = input[copyRow];
	}
	
	int line = 50000;
	while(line <= row)
	{
		//start clock
		gettimeofday(&t1, NULL);
		s = (double)(t1.tv_sec + (t1.tv_usec / 1.0e9));
		//run the bubble sort algorithm
		bubbleSort(copy, line);
		gettimeofday(&t2, NULL);
		e = (double)(t2.tv_sec + (t2.tv_usec / 1.0e9));
		//calculate the time taken, value of CLOCKS_PER_SEC is 1000, exist in the time.h library. 
		timeTaken = e - s;
		printf("\nWhen number of line = %d, ", line);
		printf("Time taken to sort the elements = %f seconds. \n", timeTaken);
		print(copy, line, timeTaken);
		
		line+= 50000;
		for(copyRow = 0; copyRow < row; copyRow++)
		{
			copy[copyRow] = input[copyRow];
		}
	}
	free(copy);
	
	printf("\n\n");
	printf("Press any key to continue......\n");
	random = getchar();
	return 0;
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

void bubbleSort(int arr[], int n)
{
	int temp = 0;
	int nr = 0;
	int a, changes;
	changes = 1;
	while(changes)
	{
		#pragma omp parallel private(temp)
		{
			nr++;
			changes = 0;
			#pragma omp for \
				reduction(+:changes)
			for(a = 0; a < n-1; a = a + 2)
			{
				if(arr[a] > arr[a+1])
				{
					temp = arr[a];
					arr[a] = arr[a+1];
					arr[a+1] = temp;
					++changes;
				}
			}
			
			#pragma omp for \
				reduction(+:changes)
			for(a = 1; a < n-1; a = a + 2)
			{
				if(arr[a] > arr[a+1])
				{
					temp = arr[a];
					arr[a] = arr[a+1];
					arr[a+1] = temp;
					++changes;
				}
			}
		}
	}
	printf("\nnr = %d", nr);
}
