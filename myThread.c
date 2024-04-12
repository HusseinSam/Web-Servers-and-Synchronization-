//
// Created by matan on 1/7/2023.
//

#include "myThread.h"




struct thread_t{

    pthread_t thread;
    int thread_index;
    int static_requests;
    int dynamic_requests;
    int total_requests;
};

my_thread my_thread_create(int thread_index,void* (*threads_function) (void*),void* arg)
{
    ///maybe this malloc is not necessarily cause we already allocated the full size in the main
    //The my_thread_create function uses malloc to allocate memory for a single my_thread struct,
    my_thread new_thread= (my_thread)malloc(sizeof(*new_thread));
    if(new_thread == NULL)
    {
        return NULL;
    }
    
    pthread_create(&(new_thread->thread), NULL, threads_function, arg);
    new_thread->thread_index = thread_index;
    //new_thread->thread = thread;
    new_thread->static_requests=0;
    new_thread->dynamic_requests=0;
    new_thread->total_requests=0;

    return new_thread ;
}

void my_thread_destroy(my_thread new_thread)
{
    ///if we figured out that we will do join for the threads then we need to destroy the struct.
    //free(new_thread);
    pthread_cancel(new_thread->thread);
    free(new_thread);
}

int my_thread_get_static_requests_num(my_thread new_thread)
{
    return new_thread->static_requests;
}

int my_thread_get_dynamic_requests_num(my_thread new_thread)
{
    return new_thread->dynamic_requests;
}
int my_thread_get_total_requests_num(my_thread new_thread)
{
    return new_thread->total_requests;
}
int my_thread_get_index(my_thread new_thread){
    return new_thread->thread_index;
}
void my_thread_increase_static_requests_num(my_thread new_thread)
{
    new_thread->static_requests++;
}
void my_thread_increase_dynamic_requests_num(my_thread new_thread)
{
    new_thread->dynamic_requests++;
}
void my_thread_increase_total_requests_num(my_thread new_thread)
{
    new_thread->total_requests++;
}