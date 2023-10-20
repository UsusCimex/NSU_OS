#define _GNU_SOURCE
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define PAGE_SIZE 4096
#define STACK_SIZE (PAGE_SIZE * 5)

void* myFunc(void* arg) {
    for (int i = 0; i < 5; ++i) {
        printf("[%d]: hello world!\n", gettid());
        usleep(10);
    }
    return NULL;
}

void* myFunc2(void* arg) {
    int* newValue = (int*)malloc(sizeof(int));
    *newValue = *((int*)arg) * 2;
    return newValue;
}

typedef struct {
    int     id;
    void    *(*start_routine)(void *);
    void    *arg;
    void    *return_value;
    void    *stack;

    volatile int finished;
    volatile int joined;
} mythread_struct;

typedef mythread_struct* mythread_t;

int thread_start(void *arg) {
    mythread_struct *mythread = (mythread_struct*)arg;

    mythread->finished = 0;
    mythread->return_value = mythread->start_routine(mythread->arg);
    mythread->finished = 1;

    while (!mythread->joined) {
        sched_yield();  // Передаем управление другим потокам (можно усыпить)
    }

    return 0;
}

int mythread_create(mythread_t *tid, void *(*start_routine)(void *), void *arg) {

    static int mythread_id = 0;
    mythread_struct* mythread;

    mythread_id += 1;

    // Create stack
    void* mythread_stack;
    mythread_stack = mmap(NULL, STACK_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (mythread_stack == MAP_FAILED) {
        perror("mmap");
        mythread_id -= 1;
        return -1;
    }

    mythread = (mythread_struct*)((char*)mythread_stack + STACK_SIZE - sizeof(mythread_struct));
    mythread->id = mythread_id;
    mythread->stack = mythread_stack;
    mythread->start_routine = start_routine;
    mythread->return_value = NULL;
    mythread->arg = arg;
    mythread->finished = 0;
    mythread->joined = 0;
    
    int flags = CLONE_VM|CLONE_FILES|CLONE_SIGHAND|
                CLONE_THREAD|CLONE_FS|SIGCHLD|
                CLONE_PARENT_SETTID|CLONE_CHILD_CLEARTID|
                CLONE_SYSVSEM;
    void* child_stack = (void*)mythread;
    int pid = clone(thread_start, child_stack, flags, mythread);
    if (pid == -1) {
        perror("clone");
        munmap(mythread->stack, STACK_SIZE);
        mythread_id -= 1;
        return -1;
    }
    usleep(15);
    
    *tid = mythread;
    return 0;
}

int mythread_join(mythread_t tid, void** returnValue) {
    mythread_struct* mythread = tid;

    while (!mythread->finished) {
        sched_yield();  // Передаем управление другим потокам (можно усыпить)
    }

    munmap(mythread->stack, STACK_SIZE);
    mythread->joined = 1;
    if (returnValue != NULL) {
        *returnValue = mythread->return_value;
    }

    return 0;
}

int main() {
    mythread_t tid;
    int result = mythread_create(&tid, myFunc, NULL);
    if (result != 0) {
        printf("Ошибка создания потока\n");
        return 1;
    }
    mythread_t tid2;
    int result2 = mythread_create(&tid2, myFunc, NULL);
    if (result2 != 0) {
        printf("Ошибка создания потока\n");
        return 1;
    }
    
    mythread_t tid3;
    int* param = (int*)malloc(sizeof(int));
    *param = 10;
    int result3 = mythread_create(&tid3, myFunc2, (void*)param);
    if (result3 != 0) {
        printf("Ошибка создания потока\n");
        return 1;
    }

    void* retValue;
    mythread_join(tid3, &retValue);
    printf("tid3 result: %d\n", *((int*)retValue));
    
    mythread_join(tid, NULL);
    mythread_join(tid2, NULL);
    
    free(param);
    free(retValue);
    return 0;
}
