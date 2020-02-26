/****************************
Program: Parallel Genetic Algorithm for Solving N Queens Puzzle
Author:
Xiang Gao (xiangg4)
Yiting Wang (charlotte)
****************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

#define N_QUEENS 10//number of queens
#define N_POPULATION 200 + 10 * N_QUEENS//size of population
#define TRUE 1
#define FALSE 0
#define EMIGRATION 3 //generation gaps for emigration
#define EMIGRATION_RATIO 0.3 //emigration ratio
#define CROSSOVER_RATIO 2 // ratio = 1/(CROSSOVER_RATIO+1)
#define MUTATE_RATIO 2 // ratio = 1/(MUTATE_RATIO+1)

/*****************   declaration   ****************/
typedef int BOOL;
struct Organism//individual in population
{
	int gene[N_QUEENS];//gene
	double adaptive;//adaption/fitness
	BOOL optimal;//optimal = TRUE: is optimal  optimal = FALSE: is not optimal
};
typedef int BOOL;
void geneticInit(struct Organism * population, int n_population);//init the population
void geneticCalculation(struct Organism * population, int n_population);//genetic algorithm calculation
void calcuAdaptive(struct Organism * population);// calculate the adaption
void choose(struct Organism * population, int n_population);// selection
void geneticCrossover(struct Organism * population, int n_population);// crossover
void geneticMutate(struct Organism * population, int n_population);// mutate
void emigration(struct Organism * population, int n_population);//emigration
struct Organism * check_optimal(struct Organism *population, int n_population);//check if this generation have an optimal individual
void print(int * optimal_solution);// print the final result
int c_lower_bound(double a[], int n, double x); //implement C++'s lower_bound() in C
void swap(int * a, int * b);//exchange the value

/****************   main   *****************/

int main(int argc, char *argv[])
{
	int rank, numproces;
	int namelen;
	int i;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	BOOL is_success = FALSE;//TRUE£ºsystem find the final result FALSE£ºsystem desn't find the final result

	srand((unsigned)time(NULL));//rand seed


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);//get the rank of process
	MPI_Comm_size(MPI_COMM_WORLD, &numproces);//get the number of processes

	MPI_Get_processor_name(processor_name, &namelen);
	printf("MPI: This is the Process %d of %d on %s\n", rank, numproces, processor_name);
	fflush(stdout);
	if (rank == 0)
	{
		printf("Number of processes: %d\n", numproces);
		printf("Number of queens: %d\n", N_QUEENS);
	}
	
	
	/*****************   initialization   *******************/
	clock_t start, finish;
	start = clock();

	int n_queens = N_QUEENS;
	int n_population_total = (N_POPULATION / numproces + 1) * numproces;
	int batch = n_population_total / numproces;
	struct Organism * population_total;
	struct Organism * population;
	int optimal_solution[N_QUEENS];//final solution

	population_total = malloc(n_population_total * sizeof(struct Organism));
	geneticInit(population_total, n_population_total);
	population = malloc(batch * sizeof(struct Organism));

	/******************	  define the C struct type for MPI   *******************/

	MPI_Datatype struct_type1;

	MPI_Aint offset1[3];

	MPI_Get_address(population_total[0].gene, &offset1[0]);
	MPI_Get_address(&(population_total[0].adaptive), &offset1[1]);
	MPI_Get_address(&(population_total[0].optimal), &offset1[2]);


	for (i = 2; i >= 0; i--)
	{
		offset1[i] -= offset1[0];
	}	

	int blockcount1[3] = { N_QUEENS, 1, 1 };

	MPI_Datatype oldtype1[3] = { MPI_INT, MPI_DOUBLE, MPI_INT };

	MPI_Type_create_struct(3, blockcount1, offset1, oldtype1, &struct_type1);
	MPI_Type_commit(&struct_type1);

	/*****************   parallel genetic calculation   *******************/

	MPI_Scatter(population_total, batch, struct_type1, population, batch, struct_type1, 0, MPI_COMM_WORLD);//divide the population to each island

	int generation = 0;

	while (!is_success)
	{
		generation++;

		geneticCalculation(population, batch);//genetic calculation

		MPI_Gather(population, batch, struct_type1, population_total, batch, struct_type1, 0, MPI_COMM_WORLD);//gather the result of a generation
		
		if (rank == 0)//check if optimal
		{
			struct Organism * organism;
			organism = check_optimal(population_total, n_population_total);
			is_success = (*organism).optimal;
			if (is_success == TRUE)
			{
				memcpy(optimal_solution, (*organism).gene, n_queens * sizeof(int));
			}

		}

		MPI_Bcast(&is_success, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(optimal_solution, N_QUEENS, MPI_INT, 0, MPI_COMM_WORLD);
		
		if (generation % EMIGRATION == 0)//emigrate on every 3 generations
		{
			emigration(population, batch);
		}

	}

	if (rank == 0)//print
	{
		print(optimal_solution);
		finish = clock();
		printf("generations: %d\n", generation);
		printf("time: %ld ms (<time.h> for windows)\n", finish - start);
	}
	/*****************   end of the program   *******************/

	MPI_Finalize();
	return 0;
}

struct Organism * check_optimal(struct Organism * population, int n_population)
{
	int i;
	for (i = 0; i < n_population; i++)
	{
		if (population[i].optimal == TRUE)
		{
			return &population[i];
		}
	}
	return &population[0];
}

void emigration(struct Organism * population, int n_population)
{
	int emi_batch = n_population * EMIGRATION_RATIO;
	int n_queens = N_QUEENS;
	int rank, numproces, i;
	struct Organism * sendbuf = malloc(emi_batch * sizeof(struct Organism));
	struct Organism * recvbuf = malloc(emi_batch * sizeof(struct Organism));
	MPI_Status status;

	/*******************   define the C struct type for MPI   *******************/
	MPI_Datatype struct_type2;

	MPI_Aint offset2[3];

	MPI_Get_address(sendbuf[0].gene, &offset2[0]);
	MPI_Get_address(&(sendbuf[0].adaptive), &offset2[1]);
	MPI_Get_address(&(sendbuf[0].optimal), &offset2[2]);

	for (i = 2; i >= 0; i--)
	{
		offset2[i] -= offset2[0];
	}

	int blockcount2[3] = { N_QUEENS, 1, 1 };

	MPI_Datatype oldtype2[3] = { MPI_INT, MPI_DOUBLE, MPI_INT };

	MPI_Type_create_struct(3, blockcount2, offset2, oldtype2, &struct_type2);
	MPI_Type_commit(&struct_type2);
	/****************   Algorithm: Fitness proportionate selection   *******************/

	double * accu_adapt = malloc(n_population * sizeof(double));

	accu_adapt[0] = population[0].adaptive;

	for (i = 1; i < n_population; i++)
	{
		accu_adapt[i] = accu_adapt[i - 1] + population[i].adaptive;
	}

	double total_adaptive = accu_adapt[n_population - 1];

	for (i = 0; i < emi_batch; i++)
	{
		//Fitness proportionate selection
		int magnify_total_adaptive = total_adaptive * 100000;    //double -> int
		int random = (rand()*rand()) % magnify_total_adaptive;//start the roulette
		double select = (double)random / 100000;            //int -> double

		int index = c_lower_bound(accu_adapt, n_population, select);
		memcpy(sendbuf[i].gene, population[index].gene, n_queens * sizeof(int));
		sendbuf[i].adaptive = population[index].adaptive;
		sendbuf[i].optimal = population[index].optimal;
	}//use Fitness proportionate selection to select the immigrant

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);//get the rank of process
	MPI_Comm_size(MPI_COMM_WORLD, &numproces);//get the number of process

	MPI_Send(sendbuf, emi_batch, struct_type2, (rank + 1) % numproces, 0, MPI_COMM_WORLD);//move out to rank + 1 island
	MPI_Recv(recvbuf, emi_batch, struct_type2, (rank - 1 + numproces) % numproces, 0, MPI_COMM_WORLD, &status);//move in from rank - 1 island


	for (i = 0; i < emi_batch; i++)//replace the individual by better one
	{
		int index = rand() % emi_batch;
		if (population[index].adaptive < recvbuf[i].adaptive)
		{
			memcpy(population[index].gene, recvbuf[i].gene, n_queens * sizeof(int));
			population[index].adaptive = recvbuf[i].adaptive;
			population[index].optimal = recvbuf[i].optimal;
		}
	}
}

void geneticInit(struct Organism * population, int n_population)
{
	int n_queens = N_QUEENS;
	int i, j;
	int * tmp_state = (int *)malloc(n_queens * sizeof(int));
	for (i = 0; i < n_population; i++)
	{
		for (j = 0; j < n_queens; j++)
		{
			tmp_state[j] = rand() % n_queens;//init
		}
		memcpy(population[i].gene, tmp_state, n_queens * sizeof(int));

		population[i].optimal = FALSE;

		calcuAdaptive(&population[i]);
		
	}
}

void calcuAdaptive(struct Organism * population)
{
	int conflict = 0;
	int n_queens = N_QUEENS;
	int * state = (*population).gene;
	int i, j;
	for (i = 0; i < n_queens; i++)
	{
		for (j = i + 1; j < n_queens; j++)
		{
			//no queens can be attacked in any same row, column or diagonal
			if (state[i] == state[j] || abs(state[i] - state[j]) == j - i)
			{
				conflict++;
			}
		}
	}

	(*population).adaptive = 1.0 / conflict;

	if (conflict == 0)
	{                     // find an optimal result
		(*population).optimal = TRUE;
	}
}

//selection (Fitness proportionate selection, avoid local optimal solution, do not choose two highest finess for crossover)
void choose(struct Organism * population, int n_population)
{
	int n_queens = N_QUEENS;
	int i;
	struct Organism * new_population = malloc(n_population * sizeof(struct Organism));;
	double * accu_adapt = malloc(n_population * sizeof(double));
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	accu_adapt[0] = population[0].adaptive;
	for (i = 1; i < n_population; i++)
	{
		accu_adapt[i] = accu_adapt[i - 1] + population[i].adaptive;
	}
	double total_adaptive = accu_adapt[n_population - 1];

	for (i = 0; i < n_population; i++)
	{
		//Fitness proportionate selection
		int magnify_total_adaptive = total_adaptive * 100000;    //double -> int
		int random = (rand()*rand()) % magnify_total_adaptive;//start the roulette
		double select = (double)random / 100000;            //int -> double

		int index = c_lower_bound(accu_adapt, n_population, select);

		memcpy(new_population[i].gene, population[index].gene, n_queens * sizeof(int));
	}
	for (i = 0; i < n_population; i++)
	{
		memcpy(population[i].gene, new_population[i].gene, n_queens * sizeof(int));
	}
}


int c_lower_bound(double a[], int n, double x) //implement C++'s lower_bound() in C
{
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


void geneticCrossover(struct Organism * population, int n_population)//crossover==>exchange gene segment (exchange queens' position)
{
	int first = 0;
	int n_queens = N_QUEENS;
	int row1;
	int i, j;
	for (i = 0; i < n_population; i++)
	{
		if (rand() % CROSSOVER_RATIO)
		{
			first++;
			if (first % 2 == 0)
			{
				int cross_point = rand() % (n_queens - 1);

				for (j = cross_point; j < n_queens; j++)
				{
					swap(&population[row1].gene[j], &population[i].gene[j]);  //exchange the value
				}
			}
			else
			{
				row1 = i;
			}
		}
	}
}

void swap(int * a, int * b)	
{
	int temp;
	temp = * a;
	* a = * b;
	* b = temp;
}

void geneticMutate(struct Organism * population, int n_population)
{
	int mutate_spot = 0;
	int n_queens = N_QUEENS;
	int i;
	for (i = 0; i < n_population; i++)
	{
		if (rand() % MUTATE_RATIO == 0)
		{
			mutate_spot = rand() % n_queens;
			population[i].gene[mutate_spot] = rand() % n_queens;
		}
		calcuAdaptive(&population[i]);  // update adaption
	}
}

void print(int * optimal_solution)
{
	int i, j;
	int n_queens = N_QUEENS;
	printf("Solution: \n");
	for (i = 0; i < n_queens; i++)
	{
		for (j = 0; j < n_queens; j++)
		{
			if (j == optimal_solution[i])
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

void geneticCalculation(struct Organism * population, int n_population)
{
	// selection
	choose(population, n_population);
	// crossover
	geneticCrossover(population, n_population);
	// mutate
	geneticMutate(population, n_population);
}
