#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct tag_pi_est_input_struct{
    uint64_t* address_of_number_in_circle;
    uint64_t my_num_of_tosses;
} pi_est_input_struct;

void* pi_est_thread_func(void* arg);
double randfrom(double min, double max);
void prepare_input_structs(pi_est_input_struct* arg,
                           uint8_t total_thread_cnt,
                           uint64_t total_toss_count);

pthread_mutex_t mutex;

int main(int argc, char* argv[])
{

    uint64_t total_toss_count = _strtoui64(argv[2] , NULL , 10);
    uint8_t number_of_threads = strtol(argv[1] , NULL , 10);
    uint64_t number_in_circle = 0;
    pthread_t* threads = (pthread_t *)malloc ( number_of_threads * sizeof ( pthread_t ));
    pi_est_input_struct* input_structs = (pi_est_input_struct*)malloc(number_of_threads * sizeof(pi_est_input_struct));
    
    struct timeval t;
    gettimeofday(&t, NULL);
    double start_time = t.tv_sec + t.tv_usec/1000000.0;
    
    pthread_mutex_init(&mutex , NULL);
    for(int thread_idx = 0; thread_idx < number_of_threads ; thread_idx++)
    {
        input_structs[thread_idx].address_of_number_in_circle = &number_in_circle;
        prepare_input_structs(&input_structs[thread_idx] , number_of_threads , total_toss_count);
        pthread_create(&threads[thread_idx] , NULL , pi_est_thread_func , (void*)&input_structs[thread_idx]);
    }
    for(int thread_idx = 0; thread_idx < number_of_threads ; thread_idx++)
    {
        pthread_join(threads[thread_idx] , NULL);
    }

    gettimeofday(&t, NULL);
    double end_time = t.tv_sec + t.tv_usec/1000000.0;

    printf("For %d threads and %llu trials, estimate of pi is: %.9f. Program lasted %f seconds" , number_of_threads , total_toss_count ,
                    4 * (double)number_in_circle / total_toss_count , end_time - start_time);
    free(input_structs);
    free(threads);
    pthread_mutex_destroy(&mutex);

}

void* pi_est_thread_func(void* arg)
{
    pi_est_input_struct* param = (pi_est_input_struct*)arg;
    uint64_t my_num_in_circle = 0;
    srand (time ( NULL));
    for(uint64_t i = 0; i < param->my_num_of_tosses; i++)
    {
        double x_coor = randfrom(-1 , 1);
        double y_coor = randfrom(-1 , 1);
        if(x_coor * x_coor + y_coor * y_coor <= 1)
        {
            my_num_in_circle++;
        }
    }
    pthread_mutex_lock(&mutex);
    *(param->address_of_number_in_circle) += my_num_in_circle;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void prepare_input_structs(pi_est_input_struct* arg,
                           uint8_t total_thread_cnt,
                           uint64_t total_toss_count)
{
    arg->my_num_of_tosses = (uint64_t)((double)total_toss_count / total_thread_cnt );
}

double randfrom(double min, double max) 
{
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}