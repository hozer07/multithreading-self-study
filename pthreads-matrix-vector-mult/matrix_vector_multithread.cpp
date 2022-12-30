#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>

int thread_count;
struct input_struct{
	int **A;
	int *x , *y;
	int row_dim , column_dim , row_per_thread;
};
void* matrix_vector_multiplication_thread ( void* param ) ;
void take_matrix_input(struct input_struct*);
void free_resources(struct input_struct*);
void print_res(struct input_struct*);
void separate_thread_inputs(input_struct* output , input_struct* input, long thread_rank);

int main ( int argc , char * argv []) {
	long thread_idx;
	pthread_t * thread_handles ;
	thread_count = strtol ( argv [1] , NULL , 10);
	struct input_struct inp_str;
	inp_str.row_dim = strtol ( argv [2] , NULL , 10);
	inp_str.column_dim = strtol ( argv [3] , NULL , 10);
	take_matrix_input(&inp_str);
	thread_handles = (pthread_t *)malloc ( thread_count * sizeof ( pthread_t ));
	auto temp = new struct input_struct[thread_count];
	for ( thread_idx = 0; thread_idx < thread_count ; thread_idx ++)
	{
		separate_thread_inputs(&temp[thread_idx] , &inp_str , thread_idx);
		pthread_create (&thread_handles [ thread_idx ] , NULL ,
			matrix_vector_multiplication_thread , ( void *) &temp[thread_idx] ) ;
	}
	for ( thread_idx = 0; thread_idx < thread_count ; thread_idx ++)
		pthread_join ( thread_handles [ thread_idx ] , NULL ) ;
	print_res(&inp_str);
	free ( thread_handles );
	delete[] temp;	
	free_resources(&inp_str);
	getchar();
	return 0 ;
}
void separate_thread_inputs(input_struct* output , input_struct* input, long thread_rank){
	int low_lim = thread_rank * ( input->row_dim / thread_count );
	int high_lim = ( thread_rank == thread_count - 1 ) ? 
					input->row_dim : (thread_rank + 1 ) * ( input->row_dim / thread_count );

	output->A = &(input->A[low_lim]);
	output->y = &(input->y[low_lim]);
	output->x = input->x;
	output->column_dim = input->column_dim;
	output->row_per_thread = high_lim - low_lim;
}

void print_res(struct input_struct* param)
{
	printf("Resulting vector is:\n");
	for(int i = 0; i< param->row_dim; i++)
	{
		std::cout << param->y[i]<<"\n";
	}
}
void free_resources(struct input_struct* param)
{
	for(int i = 0; i< param->row_dim; i++)
	{
		delete[] param->A[i];
	}
	delete[] param->A;
	delete[] param->x;
	delete[] param->y;
}
void * matrix_vector_multiplication_thread ( void * param ) {
/* Use long i n case o f 64− bit system */
	struct input_struct* t_param = (struct input_struct*) param;

	for(int i = 0; i < t_param->row_per_thread; i++)
	{
		t_param->y[i] = 0.0;
		for(int j = 0 ; j < t_param->column_dim ; j++)
		{
			t_param->y[i] += t_param->A[i][j] * t_param->x[j];
		}
	}
	return NULL ;
}

void take_matrix_input(struct input_struct* inp_str)
{
	printf("First, type the elements of the %d by %d matrix\nin their respective places and then type vector elements.\n",inp_str->row_dim,inp_str->column_dim);
	inp_str->y = new int[inp_str->row_dim];
	inp_str->A = new int*[inp_str->row_dim];
	for(int i = 0; i < inp_str->row_dim; i++)
	{
		inp_str->A[i] = new int[inp_str->column_dim];
	}
	for(int i = 0; i < inp_str->row_dim ; i++)
	{
		for(int j = 0 ; j < inp_str->column_dim; j++)
		{
			std::cin >> inp_str->A[i][j];
		}
	}

	inp_str->x = new int[inp_str->column_dim];
	for(int j = 0 ; j < inp_str->column_dim; j++)
	{
		std::cin >> inp_str->x[j];
	}
}