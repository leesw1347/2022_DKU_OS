/*
*   DKU Operating System Lab
*           Lab2 (Hash Queue Lock Problem)
*           Student id : 
*           Student name : 
*
*   lab1_sync_types.h :
*       - Lab2 header file.
*       - It contains Hash Queue structure, timeval functions, ... etc
*
*/

#ifndef _LAB2_HEADER_H
#define _LAB2_HEADER_H

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#define HASH_SIZE        13
#define LAB2_SUCCESS    0
#define LAB2_ERROR        -1
#define null NULL
#define false 0
#define true 1
#define Y true
#define N false

/*
 * If You need to more functions or vaiable
 * Declare functions or struct in lab2_sync_types.h
 */

/* Variable that contains number of data */
int target;

/* Strucuture for thread, hash, queue */
typedef struct threadArg {
    int node_count;
    int thread_num;
    pthread_mutex_t **pPthreadMutex;
} thread_arg; // Maybe you don't need to modify this structure.

typedef struct q_node {
    struct q_node *next; // 4byte pointer
    struct q_node *prev; // 4byte pointer
    int data; // 4byte
} queue_node;

queue_node *front; // 큐노드의 최앞단
queue_node *rear; // 큐노드의 최뒷단

typedef struct hash_list {
    struct hash_list *next; // 8byte pointer
    struct q_node *q_loc; // 8byte pointer
} hlist_node;

hlist_node *hashlist[HASH_SIZE];

/* Lab2 Hash and Queue init functions */
void init_queue();

void init_hlist_node();

/* Lab2 Hash function */
int hash(int val);

int value_exist(int val);

/* Lab2 Hash Queue functions for single-thread */

void hash_queue_add(hlist_node **hashtable, int val);

void hash_queue_insert_by_target(int bucketKey);

void hash_queue_delete_by_target(int bucketKey);

/* Lab2 Hash Queue functions for Coarse-grained */
void hash_queue_add_cg(hlist_node **hashtable, int val);

void hash_queue_insert_by_target_cg(int bucketKey, pthread_mutex_t *pthreadMutex);

void hash_queue_delete_by_target_cg(int bucketKey, pthread_mutex_t *pthreadMutex);

/* Lab2 Hash Queue functions for fine-grained */
void hash_queue_add_fg(hlist_node **hashtable, int val);

void hash_queue_insert_by_target_fg(int bucketKey, pthread_mutex_t *pthreadMutex);

void hash_queue_delete_by_target_fg(int bucketKey, pthread_mutex_t *pthreadMutex);

/* Lab2_timeval.c related function */
double get_timeval(struct timeval *tv_start, struct timeval *tv_end);

// crtical secition mutex list
typedef struct pm {
    pthread_mutex_t *pMutex;
} pMutexARray;

// 해시리스트만큼 뮤택스를 생성한다
pthread_mutex_t *pthreadMutex[2]; // fine-grained-lock example
pthread_mutex_t *pthreadMutex1; // coarse-grained lock

#ifndef DEBUG
#define DEBUG true
#endif

#endif /* LAB2_HEADER_H*/


