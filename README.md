# COMP90025
Project 2
1. Introduction:
We make 3 solutions for N Queens Puzzle:
A parallel solution based on parallel genetic algorithm. 
Two sequential solutions based on basic genetic algorithm and backtracking algorithm, which is used for discussion in the report. 

2. Directories: 
Project6: A Microsoft VS2017 Project Directory of the Parallel Genetic Algorithm Solution
	Name of source code: PGA.c
Project15: A Microsoft VS2017 Project Directory of the Basic Genetic Algorithm Solution
	Name of source code: GA.c
Project16: A Microsoft VS2017 Project Directory of  the Backtracking Solution
	Name of source code: BackTrack.c

3. Environment:
All these project are compiled and built on Microsoft Visual Studio 2017 (Window 10). 

4. Configuration and Run on Windows 10:
4.1 Parallel Genetic Algorithm Solution:
Step 1: Install Microsoft MPI on the computer and make a number of configurations combining MS-MPI and VS2017. (Guide: https://www.youtube.com/watch?v=IW3SKDM_yEs)
Step 2: Build the project and generate the .exe file
Step 3: Run the .exe on cmd window. 
Example of command line: "D:\Program Files\Microsoft MPI\Bin\mpiexec.exe -n 4 D:\proj2-charlotte-xiangg4\Project6\Debug\COMP90025PROJ2.exe"
Change the first and last address to your local address. 
"4" is the number of processes. You can change it.
Step 4: The number of queens is set to be 10 by default. You can change it in the 14 line of PGA.c.  The value of "#define N_QUEENS 10" can be changed to other integers. (In our experiment it is from 10 to 50)

4.2 Basic Genetic Algorithm Solution:
Step 1: Build and Run on VS2017 directly.
Step 2: The number of queens is set to be 10 by default. You can change it in the 14 line of GA.c. The value of "#define N_QUEENS 10" can be changed to other integers. (In our experiment it is from 10 to 50)

4.3 Backtracking Solution:
Step 1: Build and Run on VS2017 directly.
Step 2: The number of queens is set to be 10 by default. You can change it in the 12 line of BackTrack.c. The value of "#define N_QUEENS 10" can be changed to other integers. In our experiment it is from 10 to 30)

5. Run on Spartan (We don't recommend it):
1. Upload the PGA.c and PGA.slurm
2. Command line: "sbatch PGA.slurm"

