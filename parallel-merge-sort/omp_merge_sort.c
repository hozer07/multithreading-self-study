#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>

// Implements a parallelized version of merge sort
// using "tasking" in OpenMP and compares it to serial version

double get_time(void);
void serial_Mergesort(int list [], int lo, int hi);
void Merge(int* list, int lo, int mid, int hi);
void parallel_merge_sort_wrapper(int* list, int lo, int hi);
void parallel_Mergesort(int list [], int lo, int hi);
void compare_results(int* list1,int* list2, int len);
int* generate_random_array(int);
void Print_list(int*,int);

int main(int argc, char* argv[])
{
    int n = strtol(argv[1] , NULL , 10);
    int *serial_list = generate_random_array(n);
    int *parallel_list = (int*)malloc(n * sizeof(int));
    memcpy(parallel_list, serial_list, n * sizeof(int));
    // Print_list ( serial_list , n ) ;
    // Serial merge sort
    double duration = get_time();
    serial_Mergesort ( serial_list , 0, n - 1);
    duration = get_time() - duration;
    printf("\nSerial Merge sort took %f seconds.\n", duration);

    // Parallel merge sort
    duration = get_time();
    parallel_merge_sort_wrapper ( parallel_list , 0, n - 1);
    duration = get_time() - duration;
    printf("Parallel Merge sort took %f seconds.\n", duration);
    compare_results(serial_list, parallel_list, n);

    // Print_list ( serial_list , n ) ;
    free( serial_list ) ;
    free( parallel_list );
    return 0 ;
}

double get_time(void){
   struct timeval t;
   gettimeofday(&t, NULL);
   return (t.tv_sec + t.tv_usec/1000000.0);
}

void serial_Mergesort(int list [], int lo, int hi){
    if( lo < hi ) {
        int mid = ( lo + hi )/2;
        serial_Mergesort ( list , lo , mid ) ;
        serial_Mergesort ( list , mid +1, hi ) ;
        Merge(list, lo , mid , hi ) ;
    }
}
void Merge(int* list, int lo, int mid, int hi)
{
    int len_of_merged_array = hi - lo + 1;
    int* merged_array = (int*)malloc(len_of_merged_array * sizeof(int));
    uint16_t left_array_idx = 0, right_array_idx = 0, merged_array_cnt = 0;
    uint8_t is_length_even = ( hi - lo ) % 2;
    uint16_t left_array_len = ( is_length_even ) ? (hi - lo + 1) / 2 : mid - lo + 1;
    uint16_t right_array_len = ( is_length_even ) ? left_array_len : left_array_len - 1;
    mid = (mid == lo ) ? lo + 1 : mid + 1;
    while( right_array_idx < right_array_len && left_array_idx < left_array_len)
    {
        if( list[lo + left_array_idx] < list[mid + right_array_idx] )
        {
            merged_array[merged_array_cnt++] = list[lo + left_array_idx++];
        }
        else{
            merged_array[merged_array_cnt++] = list[mid + right_array_idx++];
        }
    }
    while( left_array_idx < left_array_len)
    {
        merged_array[merged_array_cnt++] = list[lo + left_array_idx++];
    }
    while( right_array_idx < right_array_len)
    {
        merged_array[merged_array_cnt++] = list[mid + right_array_idx++];
    }
    memcpy(list + lo, merged_array, len_of_merged_array * sizeof(int));
    free(merged_array);
}

void parallel_merge_sort_wrapper(int list [], int lo, int hi)
{
#   pragma omp parallel
{
#   pragma omp single
    parallel_Mergesort(list, lo, hi);
}
}

void parallel_Mergesort(int list [], int lo, int hi){
    if( lo < hi ) {
        int mid = ( lo + hi )/2;
#       pragma omp task
        parallel_Mergesort ( list , lo , mid );
        parallel_Mergesort ( list , mid +1, hi );
#       pragma omp taskwait        
        Merge(list, lo , mid , hi ) ;
    }
}


int* generate_random_array(int array_len){
    int min_elem, max_elem;
    printf("Enter min and max range elements of vector:\n");
    scanf("%d %d",&min_elem, &max_elem);
    srand(time(NULL));
    int scale = RAND_MAX / (max_elem - min_elem);
    int* vector = (int*)malloc(array_len * sizeof(int));
    for(int i = 0; i < array_len; i++)
    {
        vector[i] = rand() / scale + min_elem;
    }
    return vector;
}
void Print_list(int* list, int array_len)
{
    printf("\n");
    for(int i = 0; i < array_len; i++)
    {
        printf("%d ", list[i]);
        if(i % 20 == 0 && i > 0)
            printf("\n");
    }
    printf("\n");
}

void compare_results(int* list1,int* list2, int len)
{
    for(int i = 0; i < len; i++)
    {
        if(list1[i] != list2[i])
        {
            printf("Results are different.\n");
            return;
        }
    }
    printf("Results are equal.\n");
}