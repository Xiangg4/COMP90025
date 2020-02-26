/****************************
Program: Sequential Genetic Algorithm for Solving N Queens Puzzle
Author:
Xiang Gao (xiangg4)
Yiting Wang (charlotte)
****************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#define N_QUEENS 10
#define N_POPULATION 200 + 10 * N_QUEENS
#define TRUE 1
#define FALSE 0

/*****************   declaration   ****************/
typedef int BOOL;

void GeneticCalculation();//genetic algorithm calculation
void GeneticInit(int n_queens, int n_population);//init the population
double CalcuAdaptive(int * state);// calculate the adaption
void Choose();//selection
int c_lower_bound(double a[], int n, double x);//implement C++'s lower_bound() in C
void swap(int *a, int *b);//exchange the value
void Print();//print the final result
void GeneticMutate();//mutate

/*****************   init   ****************/
BOOL IsSuccess = TRUE;
int optimalSolution[N_QUEENS];
int population[N_POPULATION][N_QUEENS];
double adaptive[N_POPULATION];

/****************   main   *****************/


int main()
{
	srand((unsigned)time(NULL));//rand seed
	printf("Number of queens: %d\n", N_QUEENS);
	GeneticInit(N_QUEENS, N_POPULATION);
	GeneticCalculation();
	system("pause");
	return 0;
}

void GeneticInit(int n_queens, int n_population)
{
	int tmpState[N_QUEENS];
	for (int i = 0; i < n_population; i++)
	{
		for (int j = 0; j < n_queens; j++)
		{
			tmpState[j] = rand() % n_queens;// init
		}
		memcpy(population[i], tmpState, n_queens * sizeof(int));

		adaptive[i] = CalcuAdaptive(tmpState);
	}
}

double CalcuAdaptive(int * state)
{
	int conflict = 0;
	int n_queens = N_QUEENS;
	for (int i = 0; i < n_queens; i++)
	{
		for (int j = i + 1; j < n_queens; j++)
		{
			//no queens can be attacked in any same row, column or diagonal
			if (state[i] == state[j] || abs(state[i] - state[j]) == j - i)
			{
				conflict++;
			}
		}
	}
	if (conflict == 0)
	{                    // find an optimal result
		IsSuccess = FALSE;
		memcpy(optimalSolution, state, n_queens * sizeof(int));             //save the solution

	}
	return (1.0 / conflict);
}

//selection (Fitness proportionate selection, avoid local optimal solution, do not choose two highest finess for crossover)
void Choose()
{
	int n_population = N_POPULATION;
	int n_queens = N_QUEENS;
	int newPopulation[N_POPULATION][N_QUEENS];
	double AdaptValue[N_POPULATION];
	AdaptValue[0] = adaptive[0];
	for (int i = 1; i < n_population; i++)
	{
		AdaptValue[i] = AdaptValue[i - 1] + adaptive[i];
	}
	double totalAdaptive = AdaptValue[n_population - 1];
	//Fitness proportionate selection
	for (int i = 0; i < n_population; i++)
	{
		//Fitness proportionate selection
		int magnifyTotalAdaptive = totalAdaptive * 100000;    //double -> int
		int random = (rand()*rand()) % magnifyTotalAdaptive;//start the roulette
		double select = (double)random / 100000;     //int -> double
		int index = c_lower_bound(AdaptValue, n_population, select);
		memcpy(newPopulation[i], population[index], n_queens * sizeof(int));
	}
	memcpy(population, newPopulation, n_queens*n_population * sizeof(int));
}


int c_lower_bound(double a[], int n, double x) {
	int l = 0;
	int h = n;
	while (l < h)
	{
		int mid = (l + h) / 2;
		if (x <= a[mid])
		{
			h = mid;
		}
		else
		{
			l = mid + 1;
		}
	}
	return l;
}


void GeneticCrossover()//crossover
{
	int first = 0;
	int n_population = N_POPULATION;
	int n_queens = N_QUEENS;
	int row1;
	for (int i = 0; i < n_population; i++)
	{
		if (rand() % 2)
		{
			first++;
			if (first % 2 == 0)
			{
				int crossPoint = rand() % (n_queens - 1);

				for (int j = crossPoint; j < n_queens; j++)
				{
					swap(&population[row1][j], &population[i][j]);  //exchange the value
				}
			}
			else row1 = i;
		}
	}
}

void swap(int *a, int *b)
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}


void Print()
{
	int n_queens = N_QUEENS;
	printf("Solution: \n");
	for (int i = 0; i < n_queens; i++)
	{
		for (int j = 0; j < n_queens; j++)
		{
			if (j == optimalSolution[i])
			{
				printf("Q ");
			}
			else
			{
				printf("* ");
			}
		}
		printf("\n");
	}
}

void GeneticMutate()
{
	int mutateSpot = 0;
	int n_population = N_POPULATION;
	int n_queens = N_QUEENS;
	for (int i = 0; i < n_population; i++)
	{
		if (rand() % 2 == 0)
		{
			mutateSpot = rand() % n_queens;
			population[i][mutateSpot] = rand() % n_queens;
		}
		adaptive[i] = CalcuAdaptive(population[i]);  //update the adaption
	}
}

void GeneticCalculation()
{

	clock_t start, finish;
	start = clock();
	int generation = 0;
	while (IsSuccess)
	{
		generation++;
		//selection
		Choose();
		//crossover
		GeneticCrossover();
		//mutate
		GeneticMutate();
	}
	//print
	Print();
	finish = clock();
	printf("generations: %d\n", generation);
	printf("time: %ld ms\n", finish - start);
}
