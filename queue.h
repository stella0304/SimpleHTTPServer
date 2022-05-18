/* Yunzhi Li (880371) Project 2, COMP300023 */
#ifndef LIST_H
	#define LIST_H


typedef struct qnode qnode_t;

struct qnode {
	int newsockfd;
    char *root_path;
	qnode_t *next;
};

typedef struct {
	qnode_t *front;
	qnode_t *back;
} queue_t;

queue_t *new_queue();
void enqueue(queue_t *q, int newsockfd, char *root_path);
qnode_t *dequeue(queue_t *q);
void free_queue(queue_t *q);
void print_queue(queue_t *q);

#endif