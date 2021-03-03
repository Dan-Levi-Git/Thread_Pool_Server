#include <stdio.h>


/* Node struct of the link list. */
typedef struct node
{
    int *client_socket;
    struct node* next;
}node;

/* Queue struct. */
typedef struct queue_node
{
    struct node *head;
    struct node *tail;
} queue_node;

/* Queue API. */
queue_node* init_queue ();
void enqueu(int*clint_sock, queue_node* queue_pool);
int *Dequeue(queue_node *queue_pool);
