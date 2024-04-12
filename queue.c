#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct node_t {
    int node_data;
    TimeVal arrival_time;
    Node next_node;
};

struct queue_t {
    int queue_limit_size;
    int queue_size;
    Node head;
    Node tail;
};

Queue queue_create( int limit_size){
    Queue new_queue = (Queue)malloc(sizeof(*new_queue));
    if(new_queue == NULL)
        exit(1);
    new_queue->queue_size = 0;
    new_queue->queue_limit_size = limit_size;
    new_queue->head = NULL;
    new_queue->tail = NULL;
    return new_queue;
}

Node node_create(int data, TimeVal arrival_time){
    Node new_node = (Node)malloc(sizeof(*new_node));
    if(new_node == NULL)
        exit(1);
    new_node->node_data = data;
    new_node->arrival_time = arrival_time;
    new_node->next_node = NULL;
    return new_node;
}

bool queue_full(Queue queue){
    return queue->queue_size == queue->queue_limit_size;
}

bool queue_empty(Queue queue){
    return queue->queue_size == 0;
}

void queue_enqueue(Queue queue, int data, TimeVal arrival_time){
    if(queue_full(queue))
        return;

    Node new_node = node_create(data, arrival_time);
    if(queue_empty(queue)){
        queue->head = new_node;
    } else {
        queue->tail->next_node = new_node;
    }
    queue->tail = new_node;
    queue->queue_size++;
}


int queue_dequeue(Queue queue){
    if(queue_empty(queue))
        return -1;
    Node tmp = queue->head->next_node;
    int data = queue->head->node_data;
    free(queue->head);
    queue->head = tmp;
    if(tmp == NULL) {
        queue->tail = NULL;
    }
    queue->queue_size--;
    return data;
}

int queue_find(Queue queue, int data){
    if(queue_empty(queue))
        return -1;
    Node tmp = queue->head;
    int itr = 0;
    //int itr = 1;
    
    while(tmp){
        if(data == tmp->node_data){
            return itr;
        }
        itr++;
        tmp = tmp->next_node;
    }
    return -1;
}

int queue_dequeue_by_index(Queue queue, int index){
    if(queue_empty(queue)||index < 0 || index >= queue_get_size(queue))
        return -1;
    if(index == 0){
        return queue_dequeue(queue);
    }
    Node node_to_remove = queue->head;
    Node prev_node = NULL;
    int i = 0;
    while(i < index){
        prev_node = node_to_remove;
        node_to_remove = node_to_remove->next_node;
        i++;
    }
    int data = node_to_remove->node_data;
    prev_node->next_node = node_to_remove->next_node;
    free(node_to_remove);
    queue->queue_size--;
    if(index == queue_get_size(queue)){
        queue->tail = prev_node;
    }
    return data;
}

int queue_get_size(Queue queue){
    return queue->queue_size;
}

int queue_trans_between_2_queues(Queue from,Queue to,TimeVal time_arrival) {
    if (queue_empty(from) || queue_full(to))
        return -1;


    int data = queue_dequeue(from);
    queue_enqueue(to, data, time_arrival);
    return data;
}
struct timeval queue_head_arrival_time(Queue queue){
    return queue->head->arrival_time;
}

void queue_destroy(Queue queue){
    Node current = queue->head;
    Node next_node = NULL;
    while (current){
        next_node = current->next_node;
        free(current);
        current = next_node;
    }
    free(queue);
}

