/* Yunzhi Li (880371) Project 2, COMP300023 */
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "queue.h"

// make a new queue
queue_t *new_queue() {
    queue_t *q = malloc(sizeof(queue_t));
    assert(q != NULL);
    q->front = NULL;
    q->back = NULL;
    q->keep_working = 1;
    return q;
}

// add to queue
void enqueue(queue_t *q, int fd, char *path){
    qnode_t *new = malloc(sizeof(qnode_t));
    assert(new != NULL);

    new->newsockfd = fd;
    new->root_path = path;
    new->next = NULL;

    if (q->back == NULL) { // first insertion into queue
		q->front = new;
		q->back = new;
	} else {
		q->back->next = new;
		q->back = new;
	}
}

// remove from queue
qnode_t *dequeue(queue_t *q) {
    if (q->front == NULL) {
        return NULL;
    }
    
    qnode_t *return_node = q->front;
    q->front = q->front->next;
    if (q->back == return_node) {
        q->back = NULL;
    }

    return return_node;
}

// free the queue
void free_queue(queue_t *q) {
    assert(q != NULL);
	qnode_t *curr, *prev;
	curr = q->front; 
	while(curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
		prev = NULL;
	}
	free(q);
	q = NULL;
}

// prints a queue
void print_queue(queue_t *q) {
    assert(q != NULL);
    printf("queue:");
	qnode_t *curr;
	curr = q->front; 
	while(curr) {
		printf(" %d", curr->newsockfd);
		curr = curr->next;
	}
	printf("\n");
}

