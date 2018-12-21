
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include <sys/time.h>
double getusec_() {
        struct timeval time;
        gettimeofday(&time, NULL);
        return ((double)time.tv_sec * (double)1e6 + (double)time.tv_usec);
}

#define START timestamp = getusec_();
#define STOP(string) timestamp = getusec_() - timestamp;\
                                              timestamp = timestamp/1e6;\
                                              printf ("%s: %0.6f seconds\n",(string), timestamp);

// N,sort and merge sizes must be powers of 2
long N;
long SORT_SIZE;
long MERGE_SIZE;
#define BLOCK_SIZE 1024L


//prototype functions
void basicsort(long n, int data[n]);

void basicmerge(long n, int left[n], int right[n], int result[n*2], long start, long length);




//swap the unsorted data
//i have choose the bubble sort algorithm for this
void bubble_sort(int a[], int array_size) {
   int i, j, temp;
   for (i = 0; i < (array_size - 1); ++i) {
      for (j = 0; j < array_size - 1 - i; ++j ) {
         if (a[j] > a[j+1]) {
            temp = a[j+1];
            a[j+1] = a[j];
            a[j] = temp;
         }
      }
   }
}
//start finding the pivot
 int pivots_are_aligned(int *left, int *right, long n, long leftStart, long rightStart) {
	if (leftStart == 0 || rightStart == 0 || leftStart == n || rightStart == n) {
		return 1;
	}

	if (left[leftStart] <= right[rightStart] && right[rightStart-1] <= left[leftStart]) {
		return 1;
	}
	if (right[rightStart] <= left[leftStart] && left[leftStart-1] <= right[rightStart]) {
		return 1;
	}

	return 0;
}

 int must_decrease_left(int *left, int *right, long n, long leftStart, long rightStart) {
	return (left[leftStart] > right[rightStart]);
}

 long min(long a, long b) {
	if (a < b) {
		return a;
	} else {
		return b;
	}
}

 void find_pivot(int *left, int *right, long n, long start, long *leftStart, long *rightStart) {

	if (start == 0) {
	   *leftStart = 0;
	   *rightStart = 0;
           return;
	}
    //for pointer jumping
        int jumpSize;
        *leftStart = start/2L;
        *rightStart = start/2L;
        jumpSize = min(start/2L, n - start/2L) / 2L;

	while (1) {
		if (pivots_are_aligned(left, right, n, *leftStart, *rightStart)) {
			return;
		} else if (must_decrease_left(left, right, n, *leftStart, *rightStart)) {
			*leftStart -= jumpSize;
			*rightStart += jumpSize;
		} else {
      long leftStart, rightStart;
			*rightStart -= jumpSize;
			*leftStart += jumpSize;
		}
                jumpSize = jumpSize/2L;
                if (jumpSize==0) jumpSize=1;
	}
}

void basicmerge(long n, int left[n], int right[n], int result[n*2], long start, long length) {
	find_pivot(left, right, n, start, &leftStart, &rightStart);
//assign to the final array the offset of the current first index
	result += start;
     //while we have data in array ... do
	while (length != 0) {
		if (leftStart == n) {
			*result = right[rightStart];
			rightStart++;
			result++;
		} else if (rightStart == n) {
			*result = left[leftStart];
			leftStart++;
			result++;
		} else if (left[leftStart] <= right[rightStart]) {
			*result = left[leftStart];
			leftStart++;
			result++;
		} else {
			*result = right[rightStart];
			rightStart++;
			result++;
		}
		length--;
	}
}

void merge(long n, int left[n], int right[n], int result[n*2], long start, long length) {
        if (length < MERGE_SIZE*2L) {
                            // if the given merge size is greater than the array's length do the serial merge
		basicmerge(n, left, right, result, start, length);
        }
                        // else do two times recursive merging
       else {
                // Recursive decomposition

		#pragma omp task
                merge(n, left, right, result, start, length/2);
		#pragma omp task
                merge(n, left, right, result, start + length/2, length/2);
        }
}

void multisort(long n, int data[n], int tmp[n]) {
        if (n >= SORT_SIZE*4L) {
          // Recursive calling
// i have used task groups so no "task wait" needed
//Divide the array into four tasks and continue dividing the sub-tasks till one element remains in the array.
//each sub-task call the multisort
	#pragma omp taskgroup
		{
		#pragma omp task
                multisort(n/4L, &data[0], &tmp[0]); //0 level
		#pragma omp task
                multisort(n/4L, &data[n/4L], &tmp[n/4L]);//1 level
		#pragma omp task
                multisort(n/4L, &data[n/2L], &tmp[n/2L]);//2 level
		#pragma omp task
                multisort(n/4L, &data[3L*n/4L], &tmp[3L*n/4L]);//3 level
		}

		#pragma omp taskgroup
		{
		#pragma omp task
                merge(n/4L, &data[0], &data[n/4L], &tmp[0], 0, n/2L);
		#pragma omp task
                merge(n/4L, &data[n/2L], &data[3L*n/4L], &tmp[n/2L], 0, n/2L);
		}
//do the last merge for the hole program
		#pragma omp task
                merge(n/2L, &tmp[0], &tmp[n/2L], &data[0], 0, n);
	} else {
		//call the serial algorithm to sort the data
		basicsort(n, data);
	}
}
//call the serial algorithm to sort the data
void basicsort(long n, int data[n]){
      //i have used bubble sort algorithm for his easy implementation
                bubble_sort(data, n);
}

//initialize the data into array using rand() function
 void initialize(long length, int data[length]) {
   long i;
   for (i = 0; i < length; i++) {
         data[i] =rand();
      printf("%d\n",data[i]);
   }
}
//check if the final array is sorted
void check_sorted(long n, int data[n])
{
   int unsorted=0;
   //check serial if the data are sorted
   for (int i=1; i<n; i++){
     //data are sorted
      if (data[i-1] > data[i])
       unsorted++;
      printf("Sorted Data: %d\n",data[i]);

    }
    //data are unsorted
   if (unsorted > 0)
      printf ("\n ERROR: %d unsorted data!\n\n",unsorted);
   else {
      printf ("Ordered data! \n");
   }
}


int main(int argc, char **argv) {

  //the program needs 4 arguments to run the executable file, the sizeof array , the size of sorting  and th size of merging
        if (argc != 4) {
                fprintf(stderr, "Usage: %s <array size power of 2> <sort size power of 2> <merge size power of 2>\n", argv[0]);
                return 1;
        }

	N = atol(argv[1])*BLOCK_SIZE; //parse data to int
	SORT_SIZE = atol(argv[2]);//parse data to int
  MERGE_SIZE = atol(argv[3]);//parse data to int

	int *data = malloc(N*sizeof(int));//allocate memory
	int *tmp =  malloc(N*sizeof(int));//allocate memory

        double timestamp;
         //call the macro for the hole program time


	initialize(N, data);


   	START;

   //call the multisort function parallel
    #pragma omp parallel
    { //sychronization at the end of the block
       #pragma omp single
       multisort(N, data, tmp);
    }

   	STOP("Multisort execution time");
//call the macro for the multisort time execution

   	check_sorted (N, data);

     free(tmp);
     free(data);

    	fprintf(stdout, "End of program\n");
      fflush(stdout);
	return 0;
}
