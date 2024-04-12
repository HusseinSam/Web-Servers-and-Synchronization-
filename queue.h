#include <stdbool.h>
#include <sys/time.h>

typedef struct queue_t *Queue;
typedef struct node_t *Node;
typedef struct timeval TimeVal;

Queue queue_create( int limit_size);
Node node_create(int data, TimeVal arrival);
int queue_get_size(Queue queue);
bool queue_full(Queue queue);
bool queue_empty(Queue queue);
void queue_enqueue(Queue queue, int data, TimeVal arrival);
TimeVal queue_head_arrival_time(Queue queue);
int queue_dequeue(Queue queue);
int queue_find(Queue queue, int data);
int queue_dequeue_by_index(Queue queue, int index);
int queue_trans_between_2_queues(Queue from,Queue to,TimeVal time_arrival);
void queue_destroy(Queue queue);
