#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "custom_rw_lock.h"

void init_custom_rw_lock(my_rw_lock* );
void read_lock(my_rw_lock* );
void write_lock(my_rw_lock* );
void rw_unlock(my_rw_lock*  , read_or_write );
void destroy_my_rw_lock(my_rw_lock* );

void init_custom_rw_lock(my_rw_lock* arg)
{
    pthread_cond_init(&arg->read_cond_var , NULL);
    pthread_cond_init(&arg->write_cond_var , NULL);
    pthread_mutex_init(&arg->rwmutex , NULL);
    arg->active_reader_cnt = 0;
    arg->waiting_reader_cnt = 0;
    arg->is_a_writer_active = 0;
    arg->waiting_writer_cnt = 0;
}

void read_lock(my_rw_lock* arg)
{
    pthread_mutex_lock(&arg->rwmutex);
    if( arg->is_a_writer_active )
    {
        arg->waiting_reader_cnt++;
        pthread_cond_wait(&arg->read_cond_var , &arg->rwmutex);
        arg->waiting_reader_cnt--;
        arg->active_reader_cnt++;
    }
    else
    {
        arg->active_reader_cnt++;
    }
    pthread_mutex_unlock(&arg->rwmutex);
}

void write_lock(my_rw_lock* arg)
{
    pthread_mutex_lock(&arg->rwmutex);

    if( arg->is_a_writer_active || arg->active_reader_cnt > 0)
    {
        arg->waiting_writer_cnt++;
        pthread_cond_wait(&arg->write_cond_var , &arg->rwmutex);
        arg->waiting_writer_cnt--;
    }
    arg->is_a_writer_active = 1;
    
    pthread_mutex_unlock(&arg->rwmutex);
}

void rw_unlock(my_rw_lock* arg , read_or_write rw_type)
{
    pthread_mutex_lock(&arg->rwmutex);

    if(rw_type == READ)
    {
        arg->active_reader_cnt--;
    }
    else if(rw_type == WRITE)
    {
        arg->is_a_writer_active = 0;
    }
    // Write has priority over read
    if(arg->active_reader_cnt == 0 && arg->waiting_writer_cnt > 0)
    {
        pthread_cond_signal(&arg->write_cond_var);
    }
    else if(arg->waiting_reader_cnt > 0 && arg->waiting_writer_cnt == 0)
    {
        pthread_cond_signal(&arg->read_cond_var);
    }

    pthread_mutex_unlock(&arg->rwmutex);
}

void destroy_my_rw_lock(my_rw_lock* arg)
{
    pthread_cond_destroy(&arg->read_cond_var);
    pthread_cond_destroy(&arg->write_cond_var);
    pthread_mutex_destroy(&arg->rwmutex);
}