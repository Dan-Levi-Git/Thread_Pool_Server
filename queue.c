#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/* Alloceted a new queue. */
queue_node* init_queue ()
{
    queue_node* queue_pool = (queue_node*)malloc(sizeof(queue_node));
    queue_pool->head = NULL;
    queue_pool->tail = NULL;
    return queue_pool;
}

/* Enqueue a new task for the list. */
void enqueu(int*clint_sock, queue_node* queue_pool)
{
    node* new_node = malloc(sizeof(node));
    new_node->client_socket = clint_sock;
    new_node->next = NULL;

    if(queue_pool->tail == NULL)
    {
        queue_pool->head = new_node;
    }
    else
    {
        queue_pool->tail->next = new_node;
    }   
    queue_pool->tail = new_node;
}

/* Dequeue a new task for the list. */
int* Dequeue (queue_node* queue_pool)
{
    if(queue_pool->head == NULL)
    {
        return  NULL;
    }
    else
    {
        int *result = queue_pool->head->client_socket;
        node* temp = queue_pool->head;
        queue_pool->head = queue_pool->head->next;
        if(queue_pool->head == NULL)
        {
            queue_pool->tail = NULL;
        }
        free(temp);
        return result;
    }
}

