#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define LINE 50000

void print(int arr[], int row, double t);
void bubbleSort(int arr[], int n);
int * merge(int chunk1[], int chunk2[], int n1, int n2);

int main(int argc, char ** argv) 
{
	int row = 0;
	int temp = 0;
	FILE *file = fopen("data500k.txt", "r");
	int *input = NULL;
	char random;
	double start, end, timeTaken;
	struct timeval t1, t2;
	int *copy = malloc(row * sizeof(int));
	int copyRow;
	
	int id; //processor id
	int p; //number of processors
	int s; //chunk size per processor
	int step;	//variable used during task collection
	int m;	//used during task send.
	int *chunk = NULL; //chunk for MPI task
	int *other = NULL; //chunk for MPI task
	
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	if(id == 0)
	{
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
		for(copyRow = 0; copyRow < LINE; copyRow++)
		{
			copy[copyRow] = input[copyRow];
		}
		
		int r; //remainding tasks
		s = copyRow / p;
		r = copyRow % p;
		if(r!=0)
		{
			printf("Error: Number of processors must be able to divide number of tasks fully. \n");
			printf("Press any key to continue......\n");
			random = getchar();
			MPI_Abort(MPI_COMM_WORLD, 911);
		}
		//start clock
		gettimeofday(&t1, NULL);
		start = (double)(t1.tv_sec + (t1.tv_usec / 1.0e9));
		MPI_Bcast(&s, 1, MPI_INT, 0, MPI_COMM_WORLD);
		chunk =(int *)malloc(s * sizeof(int));
		MPI_Scatter(copy, s, MPI_INT, chunk, s, MPI_INT, 0, MPI_COMM_WORLD);
		
		bubbleSort(chunk, s);
	}
	else
	{
		MPI_Bcast(&s, 1, MPI_INT, 0, MPI_COMM_WORLD);
		chunk = (int *)malloc(s * sizeof(int));
		MPI_Scatter(copy, s, MPI_INT, chunk, s, MPI_INT, 0, MPI_COMM_WORLD);
		
		bubbleSort(chunk, s);
	}
	
	step = 1;
	while(step < p)
	{
		if(id % (2 * step) == 0)
		{
			if(id + step < p)
			{
				MPI_Recv(&m, 1, MPI_INT, id+step, 0, MPI_COMM_WORLD, &status);
				other = (int *)malloc(m * sizeof(int));
				MPI_Recv(other, m, MPI_INT, id+step, 0, MPI_COMM_WORLD, &status);
				chunk = merge(chunk, other, s, m);
				s = s + m;
			}
		}
		else
		{
			int near = id-step;
			MPI_Send(&s, 1, MPI_INT, near, 0, MPI_COMM_WORLD);
			MPI_Send(chunk, s, MPI_INT, near, 0, MPI_COMM_WORLD);
			break;
		}
		step = step * 2;
	}
	if(id == 0)
	{
		gettimeofday(&t2, NULL);
		end = (double)(t2.tv_sec + (t2.tv_usec / 1.0e9));
		//calculate the time taken, value of CLOCKS_PER_SEC is 1000, exist in the time.h library. 
		timeTaken = end - start;
		printf("\nWhen number of line = %d, ", s);
		printf("Time taken to sort the elements = %f seconds. \n", timeTaken);
		print(chunk, s, timeTaken);
			printf("\n\n");
		printf("Press any key to continue......\n");
		random = getchar();
		free(copy);
		free(chunk);
		free(other);
	}
	MPI_Finalize();
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
	int a;
	for(a = 0; a < n-1; a++)
	{
		int b;
		for(b = a + 1; b < n; b++)
		{
			if(arr[a] > arr[b])
			{
				temp = arr[a];
				arr[a] = arr[b];
				arr[b] = temp;
			}
		}
	}
}

int * merge(int chunk1[], int chunk2[], int n1, int n2)
{
	int i,j,k;
	i=0;j=0;k=0;
	int *result = NULL;
	result = (int *)malloc((n1+n2) * sizeof(int));
	
	while(i < n1 && j < n2)
	{
		if(chunk1[i] < chunk2[j])
		{
			result[k] = chunk1[i];
			i++;
		}
		else
		{
			result[k] = chunk2[j];
			j++;
		}
		k++;
	}
	if(i == n1)
	{
		while(j < n2)
		{
			result[k] = chunk2[j];
			j++;
			k++;
		}
	}
	else
	{
		while (i < n1)
		{
			result[k] = chunk1[i];
			i++;
			k++;
		}
	}
	return result;
}
