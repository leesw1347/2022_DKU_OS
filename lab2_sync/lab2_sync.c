/*
*   DKU Operating System Lab
*           Lab2 (Hash Queue Lock Problem)
*           Student id : 32181141
*           Student name : CHANGOHKIM
*
*   lab2_sync.c :
*       - lab2 main file.
*		- Thread-safe Hash Queue Lock code
*		- Coarse-grained, fine-grained lock code
*       - Must contains Hash Queue Problem function's declations.
*
*   Implement thread-safe Hash Queue for coarse-grained verison and fine-grained version.
*/

#include <aio.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab2_sync_types.h"

/*
 * TODO
 *  Implement function which init queue nodes for front and rear
 *  ( refer to the ./include/lab2_sync_types.h for front and rear nodes)
 */
void init_queue() {
    // You need to implement init_queue function.
    // 큐 초기화 (front, rear)
    printf("\n큐 초기화 함수 실행\n");
    front = (queue_node *) malloc(sizeof(queue_node));
    rear = (queue_node *) malloc(sizeof(queue_node));

    if (front != null && rear != null) {
//        printf("queue-front(%p)\n", front);
//        printf("queue-rear(%p)\n", rear);

        front->next = rear;
        front->prev = rear;
        front->data = -1;

        rear->next = front;
        rear->prev = front;
        rear->data = -1;
    }
//    printf("front(%p)\n", front);
//    printf("rear(%p)\n", rear);
}

/*
 * TODO
 *  Implement function which init hashlist(same as hashtable) node.
 */
void init_hlist_node() {
    // You need to implement init_hlist_node function.
    // 해시큐를 구현하기 위해서 HASH_SIZE(13)만큼 Bucket을 가진 node list pointer를 생성한다

    for (int i = 0; i < HASH_SIZE; i++) {
        // hlist_node 포인터 배열 내 각 포인터별로 hlist_node를 초기화 시켜준다
        hashlist[i] = (hlist_node *) malloc(sizeof(hlist_node));
    }

    // 각각의 해시버킷 포인터별로 값 초기화 (HASH_SIZE-1)만큼 반복을 돌아야
    // 해시리스트의 struct hash_list *next 포인터 값을 설정해줄 수 있음
    for (int i = 0; i < HASH_SIZE; i++) {
        if (hashlist[i] != null) {
            if (hashlist[i + 1] != null) { // hashlist bucket이 경계내에 있는지 체크한다
                hashlist[i]->next = hashlist[i + 1]; // hashlist bucket next pointer 설정
            }

            // q_loc 메모리를 동적할당하고, next, prev, data 값을 null로 채워준다
            // q_loc가 아니라, struct
            hashlist[i]->q_loc = (struct q_node *) malloc(sizeof(queue_node));
            if (hashlist[i]->q_loc != null) {
                hashlist[i]->q_loc->next = null;
                hashlist[i]->q_loc->prev = null;
                hashlist[i]->q_loc->data = -1;
            }
        }
    }

    printf("hashlist(%p)\n", hashlist);
    printf("hashlist-first(%p): 이 값은 hashlist[0](%p) 값과 동일해야함\n", *hashlist, hashlist[0]);
}

/*
 * TODO
 *  Implement function which calculate hash value with modulo operation.
 */
int hash(int val) {
    // You need to implement hash function.

    // 사용자 입력값이 들어오면 h(K)를 수행하기 위해서 모듈러 연산을 적용해서 리턴해준다
    int ret = 0;
    if (val >= 0) {
        ret = (val % HASH_SIZE) & 0xffffffff;
    }
//    printf("hash(%d) -> hashset-key(%d)\n", val, ret);
    return ret;
}

/*
 * TODO
 *  Implement function which insert the resilt of finding the location
 *  of the bucket using value to the entry and hashtable
 *
 *  @param hlist_node *hashtable		: A pointer variable containing the bucket
 *  @param int val						: Data to be stored in the queue node
 */
void hash_queue_add(hlist_node **hashtable, int val) {
    // You need to implement hash_queue_add function.
    int hash_value = hash(val);
    hashtable[hash_value]->q_loc->data = val; // hashqueue(q_loc)에 데이터 입력

    // 새로운 노드의 next 노드가 null 값이라면, 새로운 노드를 생성해준다
    if (hashtable[hash_value]->q_loc->next == null) {
        hashtable[hash_value]->q_loc->next = (queue_node *) malloc(sizeof(queue_node));
    }

    // new_node의 link를 연결해준다
    if (hashtable[hash_value]->q_loc->next != null) {
        hashtable[hash_value]->q_loc->next->data = -1;
        hashtable[hash_value]->q_loc->next->prev = null;
        hashtable[hash_value]->q_loc->next->next = null;
    }

    // 큐에 새로운 data를 넣는다
//    enqueue(hashtable[hash_value]->q_loc);
}

/*
 * TODO
 *  Implement function which insert the resilt of finding the location
 *  of the bucket using value to the entry and hashtable
 *
 *  @param hlist_node *hashtable		: A pointer variable containing the bucket
 *  @param int val						: Data to be stored in the queue node
 */
void hash_queue_add_cg(hlist_node **hashtable, int val) {
    // You need to implement hash_queue_add_cg function.
    int hash_value = hash(val);
    hashtable[hash_value]->q_loc->data = val; // hashqueue(q_loc)에 데이터 입력

    // 새로운 노드의 next 노드가 null 값이라면, 새로운 노드를 생성해준다
    if (hashtable[hash_value]->q_loc->next == null) {
        hashtable[hash_value]->q_loc->next = (queue_node *) malloc(sizeof(queue_node));
    }

    // new_node의 link를 연결해준다
    if (hashtable[hash_value]->q_loc->next != null) {
        hashtable[hash_value]->q_loc->next->data = -1;
        hashtable[hash_value]->q_loc->next->prev = null;
        hashtable[hash_value]->q_loc->next->next = null;
    }
}

/*
 * TODO
 *  Implement function which insert the resilt of finding the location
 *  of the bucket using value to the entry and hashtable
 *
 *  @param hlist_node *hashtable		: A pointer variable containing the bucket
 *  @param int val						: Data to be stored in the queue node
 */
void hash_queue_add_fg(hlist_node **hashtable, int val) {
    // You need to implement hash_queue_add_fg function.
    int hash_value = hash(val);
    hashtable[hash_value]->q_loc->data = val; // hashqueue(q_loc)에 데이터 입력

    // 새로운 노드의 next 노드가 null 값이라면, 새로운 노드를 생성해준다
    if (hashtable[hash_value]->q_loc->next == null) {
        hashtable[hash_value]->q_loc->next = (queue_node *) malloc(sizeof(queue_node));
    }

    // new_node의 link를 연결해준다
    if (hashtable[hash_value]->q_loc->next != null) {
        hashtable[hash_value]->q_loc->next->data = -1;
        hashtable[hash_value]->q_loc->next->prev = null;
        hashtable[hash_value]->q_loc->next->next = null;
    }
}

/*
 * TODO
 *  Implement function which check if the data(value) to be stored is in the hashtable
 *
 *  @param int val						: variable needed to check if data exists
 *  @return								: status (success or fail)
 */
int value_exist(int val) {
    // You need to implement value_exist function.
    int ret = false;

    int hash_value = hash(val);
    if (hashlist != null) { // 해시테이블이 포인터배열 메모리 할당여부 확인
        // 해시함수를 통해 K 값을 구했을 때, 해시값이 해시테이블 포인터배열 bucket 경계 내에 있는지 먼저 점검한다
        if (hash_value >= 0 && hash_value <= HASH_SIZE) {

            // 해시리스트 버킷 내 1번째 queue_node의 next가 null이 아니라면 해당 버킷에는 queue_node가 존재한다
            if (hashlist[hash_value]->q_loc != null) {
                ret = true;
            }
        }
    }
    return ret;
}

/*
 * TODO
 *  Implement function which find the bucket location using target
 */

void hash_queue_insert_by_target(int bucketKey) {
    // You need to implement hash_queue_insert_by_target function.
    // hash(bucketKey)에 해당하는 key에 entry를 넣는 작업을 수행한다
    // q_node에는 data가 bucketKey가 들어가도록 한다

    // 1) bucketKey를 hash(bucketKey)를 통해서 값을 가져온다
    if (value_exist(bucketKey)) {
        // hash queue에 노드를 추가한다
        int hash_value = hash(bucketKey);
        // hash(bucketKey)에 해당하는 버킷 내 q_loc가 존재하고,
        // 이걸 기반으로 q_loc에 값을 추가한다 이때 q_loc의 next 포인터와 data만을 활용한다
        if (hashlist[hash_value]->q_loc != null &&
            hashlist[hash_value]->q_loc->next == null &&
            hashlist[hash_value]->q_loc->prev == null &&
            hashlist[hash_value]->q_loc->data == -1) {

            front = (queue_node *) hashlist[hash_value]->q_loc;
            rear = (queue_node *) hashlist[hash_value]->q_loc;
            rear->next = front;
            rear->prev = null;
            front->next = null;
            front->prev = rear;
            hash_queue_add(hashlist, bucketKey); // hash_queue를 추가한다

        } else { // 버킷 내 해시에 데이터 값이 있고 값이 check entry를 통해서
            // entry가 없는 next pointer까지 접근한다
            int find_loc = false;
//            queue_node *qPrevNode = hashlist[hash_value]->q_loc; // node header
            queue_node *qNode = hashlist[hash_value]->q_loc->next; // next node
//            printf("\t\thashlist[%d]->q_loc(%p).data(%d)\n", hash_value, qPrevNode, qPrevNode->data);
//            printf("\t\thashlist[%d]->q_loc(%p).data(%d)\n", hash_value, qNode, qNode->data);

            while (!find_loc) { // entry를 찾지못할 경우 while loop를 돌린다
                // 2번째 이상 location node 위치 탐색
                if (qNode != null && qNode->data == -1) {
                    find_loc = true;

                } else {
                    if (qNode != null && qNode->next != null) {
                        qNode = qNode->next;
//                        printf("\t\thashlist[%d]->q_loc(%p).data(%d)\n", hash_value, qNode, qNode->data);
                    }
                }
            }
//            printf("\n");
            if (find_loc) {
                // 만약 노드를 찾으면 해당 노드에 bucketKey를 매칭해주고
                // 다음 노드를 설정해줄 수 있도록 값을 넣어준다
                qNode->data = bucketKey;
                qNode->next = (queue_node *) malloc(sizeof(queue_node));
                qNode->next->data = -1;
                qNode->next->prev = null;
                qNode->next->next = null;
            }
        }
    }
}

/*
 * TODO
 *  Implement function which find the bucket location using target
 */
void hash_queue_insert_by_target_cg(int bucketKey, pthread_mutex_t *pthreadMutex) {
    pthread_mutex_lock(pthreadMutex);
    if (value_exist(bucketKey)) {
        int hash_value = hash(bucketKey);
        if (hashlist[hash_value]->q_loc != null &&
            hashlist[hash_value]->q_loc->next == null &&
            hashlist[hash_value]->q_loc->prev == null &&
            hashlist[hash_value]->q_loc->data == -1) {

            front = (queue_node *) hashlist[hash_value]->q_loc;
            rear = (queue_node *) hashlist[hash_value]->q_loc;
            rear->next = front;
            rear->prev = null;
            front->next = null;
            front->prev = rear;
            hash_queue_add_cg(hashlist, bucketKey); // hash_queue를 추가한다

        } else { // 버킷 내 해시에 데이터 값이 있고 값이 check entry를 통해서
            // entry가 없는 next pointer까지 접근한다
            int find_loc = false;
//            queue_node *qPrevNode = hashlist[hash_value]->q_loc; // node header
            queue_node *qNode = hashlist[hash_value]->q_loc->next; // next node
//            printf("\t\thashlist[%d]->q_loc(%p).data(%d)\n", hash_value, qPrevNode, qPrevNode->data);
//            printf("\t\thashlist[%d]->q_loc(%p).data(%d)\n", hash_value, qNode, qNode->data);

            while (!find_loc) { // entry를 찾지못할 경우 while loop를 돌린다
                // 2번째 이상 location node 위치 탐색
                if (qNode != null && qNode->data == -1) {
                    find_loc = true;

                } else {
                    if (qNode != null && qNode->next != null) {
                        qNode = qNode->next;
//                        printf("\t\thashlist[%d]->q_loc(%p).data(%d)\n", hash_value, qNode, qNode->data);
                    }
                }
            }
//            printf("\n");
            if (find_loc) {
                // 만약 노드를 찾으면 해당 노드에 bucketKey를 매칭해주고
                // 다음 노드를 설정해줄 수 있도록 값을 넣어준다
                qNode->data = bucketKey;
                qNode->next = (queue_node *) malloc(sizeof(queue_node));
                qNode->next->data = -1;
                qNode->next->prev = null;
                qNode->next->next = null;
            }
        }
    }
    pthread_mutex_unlock(pthreadMutex);
}

/*
 * TODO
 *  Implement function which find the bucket location using target
 */
void hash_queue_insert_by_target_fg(int bucketKey, pthread_mutex_t *pthreadMutex) {
    // You need to implement hash_queue_insert_by_target_fg function.

    // Node별로 thread lock-unlock을 수행한다
    pthread_mutex_lock(pthreadMutex);
    if (value_exist(bucketKey)) {
        pthread_mutex_unlock(pthreadMutex);
//
        pthread_mutex_lock(pthreadMutex);
//
        int hash_value = hash(bucketKey);
        if (hashlist[hash_value]->q_loc != null &&
            hashlist[hash_value]->q_loc->next == null &&
            hashlist[hash_value]->q_loc->prev == null &&
            hashlist[hash_value]->q_loc->data == -1) {

            front = (queue_node *) hashlist[hash_value]->q_loc;
            rear = (queue_node *) hashlist[hash_value]->q_loc;
            rear->next = front;
            rear->prev = null;
            front->next = null;
            front->prev = rear;
            hash_queue_add_fg(hashlist, bucketKey); // hash_queue를 추가한다

        } else { // 버킷 내 해시에 데이터 값이 있고 값이 check entry를 통해서
            // entry가 없는 next pointer까지 접근한다
            int find_loc = false;
//            queue_node *qPrevNode = hashlist[hash_value]->q_loc; // node header
            queue_node *qNode = hashlist[hash_value]->q_loc->next; // next node
//            printf("\t\thashlist[%d]->q_loc(%p).data(%d)\n", hash_value, qPrevNode, qPrevNode->data);
//            printf("\t\thashlist[%d]->q_loc(%p).data(%d)\n", hash_value, qNode, qNode->data);

            while (!find_loc) { // entry를 찾지못할 경우 while loop를 돌린다
                // 2번째 이상 location node 위치 탐색
                if (qNode != null && qNode->data == -1) {
                    find_loc = true;

                } else {
                    if (qNode != null && qNode->next != null) {
                        qNode = qNode->next;
//                        printf("\t\thashlist[%d]->q_loc(%p).data(%d)\n", hash_value, qNode, qNode->data);
                    }
                }
            }
//            printf("\n");
            if (find_loc) {
                // 만약 노드를 찾으면 해당 노드에 bucketKey를 매칭해주고
                // 다음 노드를 설정해줄 수 있도록 값을 넣어준다
                qNode->data = bucketKey;
                qNode->next = (queue_node *) malloc(sizeof(queue_node));
                qNode->next->data = -1;
                qNode->next->prev = null;
                qNode->next->next = null;
            }
        }
        pthread_mutex_unlock(pthreadMutex);
    }
//    pthread_mutex_unlock(pthreadMutex);
}

/*
 * TODO
 *  Implement function which find the bucket location and stored data
 *  using target and delete node that contains target
 */
void hash_queue_delete_by_target(int bucketKey) {
    // You need to implement hash_queue_delete_by_target function.

    // 1) bucketKey에 해당하는 bucket이 hashlist 내 존재하는지 먼저 확인한다
    int find_loc = false; // 삭제할 버킷 내 entry를 찾으면 true로 설정

    if (value_exist(bucketKey)) {

        // queue-2) dequeue 함수 기능 구현
        int hash_value = hash(bucketKey);
//        printf("[%d] 버킷 내 [%d] 삭제가능유무\n", hash_value, bucketKey);

        // 삭제할 버킷키가 해시키 내에 존재하면 bucketKey가 있는 곳을 찾는다
        queue_node *qPrevNode = hashlist[hash_value]->q_loc;
        queue_node *qNode = hashlist[hash_value]->q_loc;
        while (!find_loc) {
            if (qNode != null &&
                qNode->data == bucketKey) {
                find_loc = true;
                break;
            } else {
                qPrevNode = qNode;
                qNode = qNode->next;
                if (qNode == null) {
                    break;
                }
            }
        }
        if (find_loc) { // 삭제할 entry를 찾았으면, 이전 entry와 다음 entry 값이 있는지를 체크해서
            // 연결리스트를 수정해준다

//            printf("\t삭제하기 전 노드[%d] (%p):[%d]\n", bucketKey, qPrevNode, qPrevNode->data);
//            printf("\t삭제할 전 노드[%d] (%p):[%d]\n", bucketKey, qNode, qNode->data);

            // 삭제할 노드가 next가 없으면 이전 노드에서 연결 링크만 끊어준다
            if (qNode->next == null) {
                qPrevNode->next = null;
                qNode->data = -1;
            }
                // 삭제할 노드 뒤에 데이터가 있으면, 삭제할 노드 이전 노드와 삭제할 앞 노드를 연결링크를 수정해준다
            else if (qPrevNode != null && qNode->next != null) {
                qPrevNode->next = qNode->next;
                if (qNode != null) {
                    qNode->data = -1;
                }
            }
        }
    }
}

/*
 * TODO
 *  Implement function which find the bucket location and stored data
 *  using target and delete node that contains target
 */
void hash_queue_delete_by_target_cg(int bucketKey, pthread_mutex_t *pthreadMutex) {
    // You need to implement hash_queue_delete_by_target_cg function.
    pthread_mutex_lock(pthreadMutex);
    // 1) bucketKey에 해당하는 bucket이 hashlist 내 존재하는지 먼저 확인한다
    int find_loc = false; // 삭제할 버킷 내 entry를 찾으면 true로 설정

    if (value_exist(bucketKey)) {

        // queue-2) dequeue 함수 기능 구현
        int hash_value = hash(bucketKey);
//        printf("[%d] 버킷 내 [%d] 삭제가능유무\n", hash_value, bucketKey);

        // 삭제할 버킷키가 해시키 내에 존재하면 bucketKey가 있는 곳을 찾는다
        queue_node *qPrevNode = hashlist[hash_value]->q_loc;
        queue_node *qNode = hashlist[hash_value]->q_loc;
        while (!find_loc) {
            if (qNode != null &&
                qNode->data == bucketKey) {
                find_loc = true;
                break;
            } else {
                qPrevNode = qNode;
                qNode = qNode->next;
                if (qNode == null) {
                    break;
                }
            }
        }
        if (find_loc) { // 삭제할 entry를 찾았으면, 이전 entry와 다음 entry 값이 있는지를 체크해서
            // 연결리스트를 수정해준다

//            printf("\t삭제하기 전 노드[%d] (%p):[%d]\n", bucketKey, qPrevNode, qPrevNode->data);
//            printf("\t삭제할 전 노드[%d] (%p):[%d]\n", bucketKey, qNode, qNode->data);

            // 삭제할 노드가 next가 없으면 이전 노드에서 연결 링크만 끊어준다
            if (qNode->next == null) {
                qPrevNode->next = null;
                qNode->data = -1;
            }
                // 삭제할 노드 뒤에 데이터가 있으면, 삭제할 노드 이전 노드와 삭제할 앞 노드를 연결링크를 수정해준다
            else if (qPrevNode != null && qNode->next != null) {
                qPrevNode->next = qNode->next;
                if (qNode != null) {
                    qNode->data = -1;
                }
            }
        }
    }
    pthread_mutex_unlock(pthreadMutex);
}


/*
 * TODO
 *  Implement function which find the bucket location and stored data
 *  using target and delete node that contains target
 */
void hash_queue_delete_by_target_fg(int bucketKey, pthread_mutex_t *pthreadMutex) {

    // You need to implement hash_queue_delete_by_target_fg function.

    // You need to implement hash_queue_delete_by_target_cg function.

    // 1) bucketKey에 해당하는 bucket이 hashlist 내 존재하는지 먼저 확인한다
    int find_loc = false; // 삭제할 버킷 내 entry를 찾으면 true로 설정

//    printf("hash_queue_delete_by_target_fg %p\n", pthreadMutex);
    pthread_mutex_lock(pthreadMutex);

    if (value_exist(bucketKey)) {
        pthread_mutex_unlock(pthreadMutex);

//
        pthread_mutex_lock(pthreadMutex);
//
        // queue-2) dequeue 함수 기능 구현
        int hash_value = hash(bucketKey);
//        printf("[%d] 버킷 내 [%d] 삭제가능유무\n", hash_value, bucketKey);

        // 삭제할 버킷키가 해시키 내에 존재하면 bucketKey가 있는 곳을 찾는다
        queue_node *qPrevNode = hashlist[hash_value]->q_loc;
        queue_node *qNode = hashlist[hash_value]->q_loc;
        while (!find_loc) {
            if (qNode != null &&
                qNode->data == bucketKey) {
                find_loc = true;
                break;
            } else {
                qPrevNode = qNode;
                qNode = qNode->next;
                if (qNode == null) {
                    break;
                }
            }
        }
        if (find_loc) { // 삭제할 entry를 찾았으면, 이전 entry와 다음 entry 값이 있는지를 체크해서
            // 연결리스트를 수정해준다

//            printf("\t삭제하기 전 노드[%d] (%p):[%d]\n", bucketKey, qPrevNode, qPrevNode->data);
//            printf("\t삭제할 전 노드[%d] (%p):[%d]\n", bucketKey, qNode, qNode->data);

            // 삭제할 노드가 next가 없으면 이전 노드에서 연결 링크만 끊어준다
            if (qNode->next == null) {
                qPrevNode->next = null;
                qNode->data = -1;
            }
                // 삭제할 노드 뒤에 데이터가 있으면, 삭제할 노드 이전 노드와 삭제할 앞 노드를 연결링크를 수정해준다
            else if (qPrevNode != null && qNode->next != null) {
                qPrevNode->next = qNode->next;
                if (qNode != null) {
                    qNode->data = -1;
                }
            }
        }
        pthread_mutex_unlock(pthreadMutex);
    } else {
        pthread_mutex_unlock(pthreadMutex);
    }
}

