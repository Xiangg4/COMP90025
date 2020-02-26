/****************************
Program: Sequential Backtracking Algorithm for Solving N Queens Puzzle
Author:
Xiang Gao (xiangg4)
Yiting Wang (charlotte)
****************************/

#include <stdio.h>
#include <stdlib.h>    
#include <time.h>

#define N_QUEENS 10//number of queens
#define START_POINT 1

int attack(int l, int i);
void print(int * optimal_solution);
void make(int  l);

int stack[N_QUEENS];    //save the current search path
clock_t start, finish;

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
void make(int  l)   //search the path recursively with the start point stack[l]
{
	int i;    //number of subnodes
	int  n_queens = N_QUEENS;
	if (l == n_queens + 1)
	{
		print(stack);
		finish = clock();
		printf("time: %ld ms\n", finish - start);
		system("pause");// stop here and just give the first solution
		//exit(0);    
	}
	for (i = 1; i <= n_queens; i++)
	{
		stack[l] = i;  //i is used to generate the substate (stack[l]) of stack[l-1]
		if (!attack(l, i))
		{
			make(l + 1);
		}
	}    //go back      
}

int attack(int l, int i)
{
	int k;
	for (k = 1; k < l; k++)
	{
		if (abs(l - k) == abs(stack[k] - i) || i == stack[k])
		{
			return 1;
		}
	}
	return 0;
}

void main()
{
	int start_point = START_POINT;
	printf("Number of queens: %d\n", N_QUEENS);
	start = clock();
	make(start_point);    //start from node 1, search
	system("pause");
	return;
}
