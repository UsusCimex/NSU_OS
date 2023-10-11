#define _GNU_SOURCE
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
<<<<<<< HEAD
#include <linux/futex.h>
#include <syscall.h>
=======
>>>>>>> a45517e (add kernelThreads)

#define PAGE_SIZE 4096
#define STACK_SIZE (PAGE_SIZE * 5)

<<<<<<< HEAD
=======
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

>>>>>>> a45517e (add kernelThreads)
typedef struct {
    int     id;
    void    *(*start_routine)(void *);
    void    *arg;
    void    *return_value;
    void    *stack;

<<<<<<< HEAD
    volatile int finished;
    volatile int joined;
    int futex;
=======
    int     finished;
    int     joined;
>>>>>>> a45517e (add kernelThreads)
} mythread_struct;

typedef mythread_struct* mythread_t;

<<<<<<< HEAD
int futex(int *uaddr, int futex_op, int val) {
    return syscall(SYS_futex, uaddr, futex_op, val,NULL, NULL, 0);
}

=======
>>>>>>> a45517e (add kernelThreads)
int thread_start(void *arg) {
    mythread_struct *mythread = (mythread_struct*)arg;

    mythread->finished = 0;
    mythread->return_value = mythread->start_routine(mythread->arg);
    mythread->finished = 1;

<<<<<<< HEAD
    mythread->futex = 1;
    futex(&mythread->futex, FUTEX_WAKE, 1);

    while (!mythread->joined) {
        // sched_yield();
        futex(&mythread->futex, FUTEX_WAIT, 0);
=======
    while (!mythread->joined) {
        sched_yield();  // Передаем управление другим потокам (можно усыпить)
>>>>>>> a45517e (add kernelThreads)
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
<<<<<<< HEAD
    mythread->id            = mythread_id;
    mythread->stack         = mythread_stack;
    mythread->start_routine = start_routine;
    mythread->return_value  = NULL;
    mythread->arg           = arg;
    mythread->finished      = 0;
    mythread->joined        = 0;
    mythread->futex         = 0;
=======
    mythread->id = mythread_id;
    mythread->stack = mythread_stack;
    mythread->start_routine = start_routine;
    mythread->return_value = NULL;
    mythread->arg = arg;
    mythread->finished = 0;
    mythread->joined = 0;
>>>>>>> a45517e (add kernelThreads)
    
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
<<<<<<< HEAD
=======
    usleep(15);
>>>>>>> a45517e (add kernelThreads)
    
    *tid = mythread;
    return 0;
}

int mythread_join(mythread_t tid, void** returnValue) {
    mythread_struct* mythread = tid;

    while (!mythread->finished) {
<<<<<<< HEAD
        // sched_yield();
        futex(&mythread->futex, FUTEX_WAIT, 1);
=======
        sched_yield();  // Передаем управление другим потокам (можно усыпить)
>>>>>>> a45517e (add kernelThreads)
    }

    mythread->joined = 1;
    if (returnValue != NULL) {
        *returnValue = mythread->return_value;
    }

<<<<<<< HEAD
    mythread->futex = 0;
    futex(&mythread->futex, FUTEX_WAKE, 0);

    munmap(mythread->stack, STACK_SIZE);
    return 0;
}

void* myFunc(void* arg) {
    for (int i = 0; i < 5; ++i) {
        printf("[%d]: hello world!\n", gettid());
        sleep(1);
    }
    return NULL;
}

void* myFunc2(void* arg) {
    int* newValue = (int*)malloc(sizeof(int));
    *newValue = *((int*)arg) * 2;
    usleep(500);
    return newValue;
}

=======
    return 0;
}

>>>>>>> a45517e (add kernelThreads)
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
<<<<<<< HEAD
}
=======
}
>>>>>>> a45517e (add kernelThreads)
