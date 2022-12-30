#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <sys/time.h>

double get_time(void);
void parallel_count_sort(int* a, int array_len, int thread_cnt);
void serial_count_sort(int* a, int array_len);
void get_array(int* array, int array_len);
void print_array(int* array, int array_len);
void verify_equal_results(int* parallel_result, int* serial_result, int array_len, int thread_cnt);

int main(int argc, char* argv[])
{
    int thread_cnt = strtol(argv[1] , NULL , 10);
    int array_len = strtol(argv[2] , NULL , 10);
    int* array_for_parallel = (int*)malloc( array_len * sizeof(int));
    int* array_for_serial = (int*)malloc( array_len * sizeof(int));
    get_array(array_for_parallel,array_len);
    memcpy(array_for_serial,array_for_parallel,array_len * sizeof (int));
    // Parallel Count Sort
    double t1 = get_time();
    parallel_count_sort(array_for_parallel, array_len, thread_cnt);
    double parallel_exec_time = get_time() - t1;
    // Serial Count Sort
    t1 = get_time();
    serial_count_sort(array_for_serial, array_len);
    double serial_exec_time = get_time() - t1;

    printf("Parallel Execution time (with %d threads): %f seconds.\nSerial execution time: %f seconds.\n",thread_cnt,parallel_exec_time,serial_exec_time);
    verify_equal_results(array_for_parallel , array_for_serial, array_len, thread_cnt);
    //print_array(array_for_parallel, array_len);
}

void get_array(int* array, int array_len)
{
    int min_elem, max_elem;
    printf("Enter min and max range elements of array:\n");
    scanf("%d %d",&min_elem, &max_elem);
    srand(time(NULL));
    int scale = RAND_MAX / (max_elem - min_elem);
    for(int i = 0; i < array_len; i++){
        array[i] = rand() / scale + min_elem;
    }
    printf("%d random array elements between %d and %d are generated.\n",array_len,min_elem,max_elem);
}

void print_array(int* array, int array_len)
{
    printf("Sorted array elements:\n");
    for(int i = 0; i < array_len; i++){
        printf("%d " , array[i]);
    }
}


double get_time(void){
   struct timeval t;
   gettimeofday(&t, NULL);
   return (t.tv_sec + t.tv_usec/1000000.0);
}

void parallel_count_sort(int* a, int array_len, int thread_cnt)
{
    int i , j , count;
    int* temp = malloc ( array_len * sizeof (int));
#   pragma omp parallel for default(none) shared(a,temp,array_len) private(i, j, count) num_threads(thread_cnt)
    for ( i = 0; i < array_len; i++) {
        count = 0;
        for ( j = 0; j < array_len ; j++)
        if(a[j] < a [i])
            count++;
        else if (a[j] == a[i] && j < i)
            count++;
        temp[count] = a[i] ;
    }
    memcpy ( a , temp , array_len * sizeof (int));
    free (temp);
}

void serial_count_sort(int* a, int array_len)
{
    int i , j , count;
    int* temp = malloc ( array_len * sizeof (int));
    for ( i = 0; i < array_len; i++) {
        count = 0;
        for ( j = 0; j < array_len ; j++)
        if(a[j] < a [i])
            count++;
        else if (a[j] == a[i] && j < i)
            count++;
        temp[count] = a[i] ;
    }
    memcpy ( a , temp , array_len * sizeof (int));
    free (temp);
}

void verify_equal_results(int* parallel_result_array, int* serial_result_array, int array_len, int thread_cnt)
{
    int flag = 1, i;
    double t1 = get_time();
#   pragma omp parallel for default(none) shared(parallel_result_array, serial_result_array, flag, array_len) private(i) num_threads(thread_cnt)
    for(i = 0; i < array_len; i++)
    {
        if(parallel_result_array[i] != serial_result_array[i])
            flag = 0;
    }
    double verify_exec_time = get_time() - t1;
    if(flag) printf("Results are equal.\n");
    else printf("Error! Results are not equal.\n");

    printf("It took %f seconds to verify results with %d threads.\n", verify_exec_time, thread_cnt);
}