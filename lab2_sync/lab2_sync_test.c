/*
*	DKU Operating System Lab
*	    Lab2 (Hash Queue Lock Problem)
*	    Student id : 32181141
*	    Student name : CHANGOHKIM
*
*   lab2_sync_test.c :
*       - Thread-safe Hash Queue Lock Problem Test Code.
*       - Coarse-grained, fine-grained Lock Test Code.
*
*   You can compare no-lock  result, coarse grained result and fine grained result.
*/

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab2_sync_types.h"

void lab2_sync_usage(char *cmd) {
    printf("\n Usage for %s : \n", cmd);
    printf("	-t: num thread, must be bigger than 0 ( e.g. 4) \n");
    printf("	-c: test target count, must be bigger than 0 ( e.g. 100000 ) \n");
    printf("	-l: determine lock types (e.g. 0=no-lock, 1=coarse-grained, 2=fine-grained) \n");
}

void lab2_sync_example(char *cmd) {
    printf("\n Example : \n");
    printf("	#sudo %s -t=4 -c=1000000 -l=0 \n", cmd);
    printf("	#sudo %s -t=16 -c=100000 -l=1 \n", cmd);
}

static void print_result(double ins_time, double del_time, double tot_time, int lock_type, int num_threads, int node_count) {
    char *cond[] = {"No-lock HQ", "Coarse-grained HQ", "Fine-grained HQ"};
    char *op[] = {"Insert", "Delete"};
    double time[] = {ins_time, del_time, tot_time};

    if (lock_type == 0) {
        for (int i = 0; i < 2; i++) {
            printf("\n=====  Multi Threads %s  %s experiment =====\n", cond[lock_type], op[i]);
            printf("\n Experiment Info \n");
            printf("	Test Node Count			: %d \n", node_count);
            printf("	Test Threads			: %d \n", num_threads);
            printf("	Execution Time			: %.2lf seconds \n", time[i]);
        }
        printf("\n	Total Execution Time		: %.2lf seconds \n", time[2]);
    } else if (lock_type == 1) {
        for (int i = 0; i < 2; i++) {
            printf("\n=====  Multi Threads %s  %s experiment =====\n", cond[lock_type], op[i]);
            printf("\n Experiment Info \n");
            printf("    Test Node Count         : %d \n", node_count);
            printf("    Test Threads            : %d \n", num_threads);
            printf("    Execution Time          : %.2lf seconds \n", time[i]);
        }
        printf("\n  Total Execution Time        : %.2lf seconds \n", time[2]);
    } else if (lock_type == 2) {
        for (int i = 0; i < 2; i++) {
            printf("\n===== Multi Threads %s  %s experiment =====\n", cond[lock_type], op[i]);
            printf("\n Experiment Info \n");
            printf("    Test Node Count         : %d \n", node_count);
            printf("    Test Threads            : %d \n", num_threads);
            printf("    Execution Time          : %.2lf seconds \n", time[i]);
        }
        printf("\n  Total Execution Time        : %.2lf seconds \n", time[2]);
    }
}

void *thread_job_insert(void *arg) {
    thread_arg *th_arg = (thread_arg *) arg;
    int node_c = th_arg->node_count;
    int thread_n = th_arg->thread_num;

//    printf("thread_job_insert 함수 실행\n");
//    printf("%d %d %d\n", node_c, thread_n, node_c / thread_n);
    for (int i = 0; i < node_c / thread_n; i++) {
        target = rand() % node_c;
        hash_queue_insert_by_target(target);
    }
}

void *thread_job_insert_cg(void *arg) {
    thread_arg *th_arg = (thread_arg *) arg;
    int node_c = th_arg->node_count;
    int thread_n = th_arg->thread_num;
    pthread_mutex_t *pthreadMutex2 = *th_arg->pPthreadMutex;
    for (int i = 0; i < node_c / thread_n; i++) {
        target = rand() % node_c;
        hash_queue_insert_by_target_cg(target, pthreadMutex2);
    }
}

void *thread_job_insert_fg(void *arg) {
    thread_arg *th_arg = (thread_arg *) arg;
    int node_c = th_arg->node_count;
    int thread_n = th_arg->thread_num;
    pthread_mutex_t *pthreadMutex2 = th_arg->pPthreadMutex[0]; // producer는 1번째 뮤텍스를 활용

    for (int i = 0; i < node_c / thread_n; i++) {
        target = rand() % node_c;

        // 0: producer mutex
        // 1: consumer mutex
        hash_queue_insert_by_target_fg(target, pthreadMutex2);
    }
}

void *thread_job_delete(void *arg) {
    thread_arg *th_arg = (thread_arg *) arg;
    int node_c = th_arg->node_count;
    int thread_n = th_arg->thread_num;

    for (int i = 0; i < node_c / thread_n; i++) {
        target = rand() % node_c;
        hash_queue_delete_by_target(target);
    }
}

void *thread_job_delete_cg(void *arg) {
    thread_arg *th_arg = (thread_arg *) arg;
    int node_c = th_arg->node_count;
    int thread_n = th_arg->thread_num;
    pthread_mutex_t *pthreadMutex2 = *th_arg->pPthreadMutex;

    for (int i = 0; i < node_c / thread_n; i++) {
        target = rand() % node_c;
        hash_queue_delete_by_target_cg(target, pthreadMutex2);
    }
}

void *thread_job_delete_fg(void *arg) {
    thread_arg *th_arg = (thread_arg *) arg;
    int node_c = th_arg->node_count;
    int thread_n = th_arg->thread_num;
    pthread_mutex_t *pthreadMutex2 = th_arg->pPthreadMutex[1]; // consmuser는 2번째 뮤텍스를 활용

    for (int i = 0; i < node_c / thread_n; i++) {
        target = rand() % node_c;
        hash_queue_delete_by_target_fg(target,pthreadMutex2);
    }
}

/**
 * print hash queue
 */
void print_hashqueue(hlist_node **hashlist) {
    if (hashlist == null) {
        return;
    }
    // hashlist는 포인터배열 주소
    // *hashlist는 포인터배열 주소 내 0번째 인덱스 포인터 주소
    for (int i = 0; i < HASH_SIZE; i++) {
        if (hashlist[i] != null) { // hashlist[i]가 null이 아니라면 내부에 존재하는 bucket과 entry 출력
            // 1) 버킷출력
            printf("bucket[%d]\n", i);
            // 2) 버킷 내 entry 출력
            int loop = false;
            queue_node *qLoop = hashlist[i]->q_loc;
            while (!loop) {
                if (qLoop != null) {
                    printf("\t(%p):[%d]\n", qLoop, qLoop->data);
                    qLoop = qLoop->next;
                } else {
                    loop = true;
                }
            }
        }
    }
}


void hq_test(int num_threads, int node_count, int lock_type) {
    struct timeval tv_insert_s, tv_insert_e, tv_delete_s, tv_delete_e, tv_total_s, tv_total_e;
    double exe_insert, exe_delete, exe_total;
    const int num_t = num_threads;
    const int node_c = node_count;
    const int lock_t = lock_type;
    pthread_t threads[num_t];

    thread_arg *th_arg;
    th_arg = (thread_arg *) malloc(sizeof(thread_arg));

    /*
     *  Multi Thread Insert and Delete Test No-lock
     */
    if (lock_type == 0) {
        gettimeofday(&tv_total_s, NULL);
        printf("\n");

        init_hlist_node();
        init_queue();

        gettimeofday(&tv_insert_s, NULL);
        for (int i = 0; i < num_t; i++) {
            th_arg->node_count = node_c;
            th_arg->thread_num = num_t;
            pthread_create(&threads[i], NULL, thread_job_insert, (void *) th_arg);
        }

        for (int i = 0; i < num_t; i++)
            pthread_join(threads[i], NULL);
        gettimeofday(&tv_insert_e, NULL);
//        print_hashqueue(hashlist);

        gettimeofday(&tv_delete_s, NULL);
        for (int i = 0; i < num_t; i++) {
            th_arg->node_count = node_c;
            th_arg->thread_num = num_t;

            pthread_create(&threads[i], NULL, thread_job_delete, (void *) th_arg);
        }

        for (int i = 0; i < num_t; i++)
            pthread_join(threads[i], NULL);
// 해시버킷 구조출력
//        print_hashqueue(hashlist);

        gettimeofday(&tv_delete_e, NULL);

        gettimeofday(&tv_total_e, NULL);
        exe_insert = get_timeval(&tv_insert_s, &tv_insert_e);

        exe_delete = get_timeval(&tv_delete_s, &tv_delete_e);
        exe_total = get_timeval(&tv_total_s, &tv_total_e);
        print_result(exe_insert, exe_delete, exe_total, lock_t, num_t, node_c);
    }

    /*
     *  Multi Threads Insert and Delete Test Coarse-grained
     */

    if (lock_type == 1) {
        // 뮤텍스 설정 및 초기화
        pthreadMutex1 = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(pthreadMutex1, null);
        th_arg->pPthreadMutex = (pthread_mutex_t **) malloc(sizeof(pthread_mutex_t *));
        // thread argument로 넘기기 위해서 th
//        printf("쓰레드 이중포인터 리스트 %p\n", th_arg->pPthreadMutex);
        *th_arg->pPthreadMutex = pthreadMutex1;
//        printf("뮤텍스 쓰레드 포인터배열에서 1번째 뮤텍스 값은 (%p)\n", *th_arg->pPthreadMutex);

        gettimeofday(&tv_total_s, NULL);
        printf("\n");

        init_hlist_node();
        init_queue();

        gettimeofday(&tv_insert_s, NULL);

        // coarse-grained unlock
//        pthread_mutex_trylock(*th_arg->pPthreadMutex);

        for (int i = 0; i < num_t; i++) {
            th_arg->node_count = node_c;
            th_arg->thread_num = num_t;
            pthread_create(&threads[i], NULL, thread_job_insert_cg, (void *) th_arg);
        }
        for (int i = 0; i < num_t; i++)
            pthread_join(threads[i], NULL);

//        print_hashqueue(hashlist);
        // coarse-grained unlock
//        pthread_mutex_unlock(*th_arg->pPthreadMutex);
        gettimeofday(&tv_insert_e, NULL);

        gettimeofday(&tv_delete_s, NULL);
        for (int i = 0; i < num_t; i++) {
            th_arg->node_count = node_c;
            th_arg->thread_num = num_t;

            pthread_create(&threads[i], NULL, thread_job_delete_cg, (void *) th_arg);
        }

        for (int i = 0; i < num_t; i++)
            pthread_join(threads[i], NULL);
        gettimeofday(&tv_delete_e, NULL);

        gettimeofday(&tv_total_e, NULL);
        exe_insert = get_timeval(&tv_insert_s, &tv_insert_e);

        exe_delete = get_timeval(&tv_delete_s, &tv_delete_e);
        exe_total = get_timeval(&tv_total_s, &tv_total_e);
        print_result(exe_insert, exe_delete, exe_total, lock_t, num_t, node_c);

        printf("\n뮤텍스 삭제 [%p]\n", pthreadMutex1);
        pthread_mutex_destroy(pthreadMutex1);
    }

    /*
     *  Multi Threads Insert and Delete Test Fine-grained
     */
    if (lock_type == 2) {
        gettimeofday(&tv_total_s, NULL);
        printf("\n");

        // 각 쓰레드에서 사용할 producer, consumer mutex를
        pthreadMutex[0] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
        pthreadMutex[1] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));

        init_hlist_node();
        init_queue();

        gettimeofday(&tv_insert_s, NULL);
        for (int i = 0; i < num_t; i++) {
            th_arg->node_count = node_c;
            th_arg->thread_num = num_t;
            th_arg->pPthreadMutex = pthreadMutex;
            pthread_create(&threads[i], NULL, thread_job_insert_fg, (void *) th_arg);
        }

        for (int i = 0; i < num_t; i++)
            pthread_join(threads[i], NULL);
        gettimeofday(&tv_insert_e, NULL);

        gettimeofday(&tv_delete_s, NULL);
        for (int i = 0; i < num_t; i++) {
            th_arg->node_count = node_c;
            th_arg->thread_num = num_t;
            th_arg->pPthreadMutex = pthreadMutex;
            pthread_create(&threads[i], NULL, thread_job_delete_fg, (void *) th_arg);
        }

        for (int i = 0; i < num_t; i++)
            pthread_join(threads[i], NULL);
        gettimeofday(&tv_delete_e, NULL);

        gettimeofday(&tv_total_e, NULL);
        exe_insert = get_timeval(&tv_insert_s, &tv_insert_e);

        exe_delete = get_timeval(&tv_delete_s, &tv_delete_e);
        exe_total = get_timeval(&tv_total_s, &tv_total_e);
        print_result(exe_insert, exe_delete, exe_total, lock_t, num_t, node_c);

        pthread_mutex_destroy(pthreadMutex[0]);
        pthread_mutex_destroy(pthreadMutex[1]);
    }

    printf("\n");
}

int main(int argc, char *argv[]) {
    int n;
    char junk;
    int num_threads = 0, node_count = 0, lock_type = 0;

    for (int i = 1; i < argc; i++) {
        if (sscanf(argv[i], "-t=%d%c", &n, &junk) == 1) {
            num_threads = n;
        } else if (sscanf(argv[i], "-c=%d%c", &n, &junk) == 1) {
            node_count = n;
        } else if (sscanf(argv[i], "-l=%d%c", &n, &junk) == 1) {
            lock_type = n;
        } else goto INVALID_ARGS;
    }
    if ((num_threads > 0) && (node_count > 0)) {
        hq_test(num_threads, node_count, lock_type);
    } else goto INVALID_ARGS;

    return LAB2_SUCCESS;

    INVALID_ARGS:
    lab2_sync_usage(argv[0]);
    lab2_sync_example(argv[0]);

    return LAB2_ERROR;
}

