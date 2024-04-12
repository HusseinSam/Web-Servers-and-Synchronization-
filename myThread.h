//
// Created by matan on 1/7/2023.
//

#ifndef WEBSERVER_FILES_MYTHREAD_H
#define WEBSERVER_FILES_MYTHREAD_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct thread_t *my_thread;
my_thread my_thread_create(int id, void* (*thread_main) (void*),void* arg);
void my_thread_destroy(my_thread new_thread);
int my_thread_get_static_requests_num(my_thread new_thread);
int my_thread_get_dynamic_requests_num(my_thread new_thread);
int my_thread_get_total_requests_num(my_thread new_thread);
int my_thread_get_index(my_thread new_thread);
void my_thread_increase_static_requests_num(my_thread new_thread);
void my_thread_increase_dynamic_requests_num(my_thread new_thread);
void my_thread_increase_total_requests_num(my_thread new_thread);
#endif //WEBSERVER_FILES_MYTHREAD_H
