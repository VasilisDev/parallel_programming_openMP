/****************************************

@Author:Vasilis Tsakiris
@Register Number: cs151082

******************************************/

#include <omp.h>
#include <cmath>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


bool check_diagoanally_dominant_parallel(int** matrix, int matrix_size);
void init_array_parallel(int array[], int array_size);
void delete_matrix(int** matrix, int matrix_size);
void delete_matrix_b(int** b, int matrix_size);
void  min_max_matrix(int** matrix, int matrix_size);


int main(){
	// Just to simulate a real program
	int user_choice = 1;
	double start_time, end_time;

	while (user_choice == 1)
	{
		int matrix_size,t;

	 //Ask for num of threads
	 printf("Enter the number of threads:\n");
	 scanf("%d",&t);
	 omp_set_num_threads(t);


		// Entering the matrix size ..
		printf("Enter the matrix size: ");
			scanf("%d", &matrix_size);


	/*	int **matrix = (int **)malloc(matrix_size* sizeof(int *));
	     for (int i=0; i<matrix_size; i++)
	          matrix[i] = (int *)malloc(matrix_size * sizeof(int));*/

		// Initializing the main structures ..
		int** matrix = new int*[matrix_size];
		for (int i = 0; i < matrix_size; i++)
		   matrix[i] = new int[matrix_size];

		// Entering the matrix elements ..
		printf("Enter the matrix elements (Enter element by element and row by row from the matrix top left corner to its end by pressing ENTER):\n");
		for (int i = 0; i < matrix_size; i++){
			printf("Row #%d elements:------\n", i);
			for (int j = 0; j < matrix_size; j++) {
				printf("Matrix[%d][%d]:\n", i, j);
				scanf("%d", &matrix[i][j]);
			}
		}
		// Printing the matrix (As a confirmation for the user) ..
		printf("The matrix in its final shape: \n");
		for (int i = 0; i < matrix_size; i++) {
			for (int j = 0; j < matrix_size; j++) {
				printf("%d\t", matrix[i][j]);
			}
			printf("\n");
		}

		// Checking if the matrix is diagonally dominant or not ..
		start_time = omp_get_wtime();
		if (!check_diagoanally_dominant_parallel(matrix, matrix_size)){
			printf("The matrix is not dominant, please enter another matrix.\n");


			// We don't need the matrix if it's not diagonally dominant ..
			// Cleaning the matrix
			delete_matrix(matrix, matrix_size);

			printf("Do you want to continue? 1/0\n");
			scanf("%d", &user_choice);
			continue;
		}
		//if the matrix isn't diagonally dominant find the absolute max value of the diagonal
		else{
			printf("The matrix is dominant.\nChecking for max value of diagonal dominant...\n");
		  min_max_matrix(matrix, matrix_size);

    }
		end_time = omp_get_wtime();
	  printf("The hole program took %g seconds\n", end_time - start_time);


		// Cleaning the matrix
		delete_matrix(matrix, matrix_size);

		printf("Do you want to continue? 1/0\n");
		scanf("%d", &user_choice);
	}
}

bool check_diagoanally_dominant_parallel(int** matrix, int matrix_size){
	// This is to validate that all the rows applies the rule ..
	int check_count = 0;

#pragma omp parallel
   {
		// For each row
		// Each thread will be assigned to run on a row.
		#pragma omp  for schedule (guided, 1)
		for (int i = 0; i < matrix_size; i++){
			float row_sum = 0;
			// Summing the other row elements ..
			for (int j = 0; j < matrix_size; j++) {
				if (j != i)
				   row_sum += abs(matrix[i][j]);
			}
			if (abs(matrix[i][i]) >= row_sum){
        //lock thread
				#pragma omp atomic
				check_count++;
			}
		}
	}

	return check_count == matrix_size;
}

void min_max_matrix(int** matrix, int matrix_size){

	int  m, min, imin, jmin;

	//initialize temporary the the first value of the matrix as the maximum value
	m  = matrix[0][0];

#pragma omp parallel
{
		 // For each row
		// Each thread will be assigned to run on a row.
	#pragma omp  for schedule (guided, 1) reduction(max : m)
		for (int  i = 0; i < matrix_size; i++){
			for (int  j = 0; j < matrix_size; j++) {
	   			{
						//finding the maximum value of the diagonally dominant matrix
						if (m < matrix[i][i]) {
							#pragma omp critical
						 {
							 if (m < matrix[i][i]) {
							m = abs(matrix[i][i]);
						  }
						}
					}
				}
	    }
 	  }
  }
 	 printf("The maximum value of the dominant diagonally matrix is ------> %d",m);
      printf("\n");

			// Initializing the new array ..
			/*
			int **b = (int **)malloc(matrix_size* sizeof(int *));
				 for (int i=0; i<matrix_size; i++)
							b[i] = (int *)malloc(matrix_size * sizeof(int));
							*/

    int** b = new int*[matrix_size];
			  for (int i = 0; i < matrix_size; i++)
				  	b[i] = new int[matrix_size];
					  // Entering the matrix elements ..
					    printf("Initializing the data of new array...\n");

   #pragma omp parallel
	 {
							// For each row
						 // Each thread will be assigned to run on a row.
					 #pragma omp  for schedule (guided, 1)
					      for (int i = 0; i < matrix_size; i++){
						     printf("Row #%d elements:------\n", i);
						      for (int j = 0; j < matrix_size; j++) {
						      	printf("Loading...\n");
						       if(i != j)
									 #pragma omp critical
									{
										if (i != j) {
											b[i][j]= m - abs(matrix[i][j]);
								  	 }
								  }
				       			else
							    b[i][j]=m;
						    }
					    }
						}


					// Printing the new matrix (b) ..
			 	printf("The new matrix (b) in its final shape: \n");
			 		for (int i = 0; i < matrix_size; i++) {
			 			for (int j = 0; j < matrix_size; j++) {
			 				printf("%d\t", b[i][j]);
			 	 		}
			 	 		printf("\n");
			 		}
					//initialize temporary the the first value of the matrix as the minimum value
					min = b[0][0];


      #pragma omp parallel
			{
					// For each row
				 // Each thread will be assigned to run on a row.
				 #pragma omp  for schedule (guided, 1)
				 for (int  i = 0; i < matrix_size; i++){
					 for (int j = 0; j < matrix_size; j++) {
         //if we have duplicate minimum value the program will find the first position of this value
						 if (min > b[i][j]) {
							 //thread lock
	             #pragma omp critical
							 {
	 				        if (min > b[i][j]) {
										 //min value
	 						     min = b[i][j];
									  //i index of minimum value
	 						     imin = i;
									 //j index of minimun value
	 						     jmin = j;
	 					      }
							  }
      	 		  }
					  }
				  }
				}
				//print the minimum value and the position which is founded
				printf("The minimum value of b matrix is ----> %d and finding in b[%d][%d] position \n",min,imin,jmin);
				delete_matrix_b(b,matrix_size);
}

//initialize the array if it wanted *optional
void init_array_parallel(int array[], int array_size){
	#pragma omp parallel for schedule (dynamic, 1)
	for (int i = 0; i < array_size; i++) {
		array[i] = 0;
	}
}

//delete the array when the main program finish
/*void delete_matrix(int** matrix, int matrix_size) {
	for (int i = 0; i < matrix_size; i++) {
	   free(matrix[i]);
	}
	free(matrix);
}*/

void delete_matrix(int** matrix, int matrix_size) {
	for (int i = 0; i < matrix_size; i++) {
		delete[] matrix[i];
	}
	delete[] matrix;
}

void delete_matrix_b(int** b, int matrix_size) {
	for (int i = 0; i < matrix_size; i++) {
	   delete [] b[i];
	}
	delete [] b;
}
