
// A simple version of "Read-Write lock"
#ifndef CUSTOM_RW_LOCK_HEADER
#define CUSTOM_RW_LOCK_HEADER

#include <stdint.h>
#include <pthread.h>

typedef enum tag_read_or_write
{
    READ,
    WRITE
} read_or_write ;

typedef struct tag_my_rw_lock
{
    pthread_cond_t read_cond_var;
    pthread_cond_t write_cond_var;
    pthread_mutex_t rwmutex;
    uint16_t active_reader_cnt;
    uint16_t waiting_reader_cnt;
    uint8_t is_a_writer_active;
    uint16_t waiting_writer_cnt;
} my_rw_lock;

void init_custom_rw_lock(struct tag_my_rw_lock* );
void read_lock(struct tag_my_rw_lock* );
void write_lock(struct tag_my_rw_lock* );
void rw_unlock(struct tag_my_rw_lock*  , read_or_write );
void destroy_my_rw_lock(struct tag_my_rw_lock* );

#endif