#include "segel.h"
#include "request.h"
#include "queue.h"
#include <pthread.h>
#include <stdlib.h>
#include "myThread.h"
#include <stdio.h>
#include <string.h>
// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <port_num (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

///mutex and queues and threads initialaization

///mutex
pthread_mutex_t mutex_lock;

///conditions
pthread_cond_t block_algo_cond;
pthread_cond_t not_empty_waiting_queue_cond;

///threads

my_thread* my_threads=NULL;

///queues

Queue waiting_queue = NULL;  ///its size is equal to the num of requests that we can get at the same time
Queue working_queue = NULL;  ///its size should be equal to the num of threads that can be working at the same time


// HW3: Parse the new arguments too
///change this function to take the 4 arguments listed in the hw:
///  [port_num] [threads_num] [queue_size] [sched_algo]
void getargs(int *port_num, int *threads_num,  int *queue_size, char *sched_algo ,int argc, char *argv[])
{
    if (argc < 5) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }

    *port_num = atoi(argv[1]);
    *threads_num   = atoi(argv[2]);
    ///these 2 values should be unsigned int , means >=0
    if( (*port_num <0) || (*threads_num <0) )
        exit(1);
    *queue_size =  atoi(argv[3]);


    strcpy(sched_algo ,argv[4] );

}


///the thread function;
void * threads_function(void* ptr){
    //int i =*((int *)ptr);
    while(1){

        pthread_mutex_lock(&mutex_lock);
        while(queue_empty(waiting_queue)){
            pthread_cond_wait(&not_empty_waiting_queue_cond, &mutex_lock);
        }
        TimeVal arrival_time = queue_head_arrival_time(waiting_queue);
        int conn_fd= queue_trans_between_2_queues(waiting_queue,working_queue,arrival_time);
        pthread_mutex_unlock(&mutex_lock);

        // handle a request
        requestHandle(conn_fd, arrival_time, *(my_thread*)ptr);
        Close(conn_fd);


        pthread_mutex_lock(&mutex_lock);
        queue_dequeue_by_index(working_queue, queue_find(working_queue,conn_fd));
        pthread_cond_signal(&block_algo_cond);
        pthread_mutex_unlock(&mutex_lock);

    }

}

int main(int argc, char *argv[])
{

    int listenfd, connfd,clientlen;
    struct sockaddr_in clientaddr;

    int port;
    int threads_num;
    int queue_size;
    char sched_algo[8]; // Declare sched_algo as a pointer to char


    getargs(&port, &threads_num, &queue_size , sched_algo ,argc, argv);


    // 
    // HW3: Create some threads...
    //
    ///queues
    waiting_queue = queue_create(queue_size);
    working_queue = queue_create(threads_num);

    ///mutexs

    pthread_mutex_init(&mutex_lock, NULL);
    ///conditions
    pthread_cond_init(&block_algo_cond, NULL);
    pthread_cond_init(& not_empty_waiting_queue_cond, NULL);

    ///threads


    //malloc a threads array
    //pthread_t* threads = malloc(threads_num * sizeof(pthread_t));

    // malloc a my_threads array
    my_thread *my_threads = malloc(threads_num * sizeof(my_threads));

    // Check for allocation errors
    /*if (threads == NULL || my_threads == NULL)
    {
        exit(1);
    }*/

    for ( int i=0; i<threads_num; i++) 
    {
        //creating th thread number i:
        /*if (pthread_create(&threads[i], NULL, threads_function, (void *)&i) != 0)
            exit(1);*/

        //creating the my_thread struct to the thread number i and
        // add the new my_thread to the my_threads array
        my_threads[i]=  my_thread_create( i,&threads_function,&my_threads[i]);
    }


    ///queues


    listenfd = Open_listenfd(port);
    while (1) 
    {
	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

	    //
	    // HW3: In general, don't handle the request in the main thread.
	    // Save the relevant info in a buffer and have one of the worker threads
	    // do the work.
	    //


        ///buffers are full we need to do block / drop tail/ drop head /block flush / drop random

        //get the arrival time of the new request
        pthread_mutex_lock(&mutex_lock);
        struct timeval arrival_time;
        gettimeofday(&arrival_time,NULL);

        //if buffers are full
        if(queue_get_size(working_queue) + queue_get_size(waiting_queue) == queue_size) 
        {

            if(strcmp(sched_algo, "block") == 0)
            {
                //block : your code for the listening (main) thread should block (not busy wait!) until a
                //buffer becomes available.
                while(queue_get_size(working_queue) + queue_get_size(waiting_queue) == queue_size) {
                    pthread_cond_wait(&block_algo_cond, &mutex_lock);
                }
            }
                //queue_enqueue(waiting_queue, connfd,  arrival_time);
                //pthread_cond_signal(&not_empty_waiting_queue_cond);
                //pthread_mutex_unlock(&mutex_lock);
                //continue;

            else if(strcmp("dt", sched_algo)==0)
            {
                //your code should drop the new request immediately by closing the socket
                //and continue listening for new requests.
                close(connfd);
                pthread_mutex_unlock(&mutex_lock);
                continue;

            }
            else if(strcmp("dh", sched_algo)==0)
            {
                //your code should drop the oldest request in the queue that is not
                //currently being processed by a thread and add the new request to the end of the
                //queue.
                if(queue_empty(waiting_queue)) { // all the threads are working on all the req , no req are waiting
                    close(connfd);
                    pthread_mutex_unlock(&mutex_lock);
                    continue;
                }
                close(queue_dequeue(waiting_queue)); // remove the oldest req and close its socket
                //queue_enqueue(waiting_queue, connfd,  arrival_time);
                //pthread_cond_signal(&not_empty_waiting_queue_cond);
                //pthread_mutex_unlock(&mutex_lock);
                //continue;
            }else if(strcmp("random", sched_algo)==0){
                if(queue_empty(waiting_queue)){
                    Close(connfd);
                    pthread_mutex_unlock(&mutex_lock);
                    continue;
                }
                else{
                    int half = (int)(queue_get_size(waiting_queue) + 1) / 2;
                    while (half > 0 && !queue_empty(waiting_queue)) {
                        int random_index = rand() % queue_get_size(waiting_queue);
                        Close(queue_dequeue_by_index(waiting_queue, random_index));
                        half--;
                    }
                }
            }

        }

        queue_enqueue(waiting_queue, connfd,  arrival_time);
        //give a signal to the threads that there is a request in the waiting queue , the queue is not empty;
        pthread_cond_signal(&not_empty_waiting_queue_cond);
        pthread_mutex_unlock(&mutex_lock);

    }
    pthread_mutex_destroy(&mutex_lock);
    pthread_cond_destroy(&not_empty_waiting_queue_cond);
    pthread_cond_destroy(&block_algo_cond);
    queue_destroy(waiting_queue);
    queue_destroy(working_queue);

    for(int i=0; i<threads_num; i++)
    {
        my_thread_destroy(my_threads[i]);
    }

}


