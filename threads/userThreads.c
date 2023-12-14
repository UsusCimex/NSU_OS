#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ucontext.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define STACK_SIZE (PAGE_SIZE * 5)
#define MAX_THREADS 4

typedef struct {
    void (*start_routine)(void *);
    void *arg;
    int alive; // 0 if finished

    ucontext_t uctx;
} uthread_struct;

typedef uthread_struct* uthread_t;

uthread_t uthreads[MAX_THREADS];
int uthread_count = 0;
int uthread_cur = 0;

void scheduler() {
    int err;
    ucontext_t* cur_ctx;
    ucontext_t* next_ctx;
    cur_ctx = &(uthreads[uthread_cur]->uctx);
    uthread_cur = (uthread_cur + 1) % uthread_count;
    next_ctx = &(uthreads[uthread_cur]->uctx);

    err = swapcontext(cur_ctx, next_ctx);
    if (err == -1) {
        printf("scheduler: swapcontext() failed: %s\n", strerror(errno));
        exit(1);
    }
}

void start_thread() {
    ucontext_t* ctx;
    for (int i = 1; i < uthread_count; ++i) { //thread 0 = main thread
        ctx = &uthreads[i]->uctx;
        char *stack_from = ctx->uc_stack.ss_sp;
        char *stack_to = ctx->uc_stack.ss_sp + ctx->uc_stack.ss_size;
        if (stack_from <= (char *)&i && (char *)&i <= stack_to) {
            uthreads[i]->start_routine(uthreads[i]->arg);
            uthreads[i]->alive = 0;
            scheduler();
        }
    }
}

void uthread_create(uthread_t* ut, void (*thread_func)(void *), void *arg) {
    char *stack;
    uthread_t new_ut;
    int err;
    stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    new_ut = (uthread_t)(stack + STACK_SIZE - sizeof(uthread_struct));
    err = getcontext(&new_ut->uctx);
    if (err == -1) {
        printf("uthread_create: getcontext() failed: %s\n", strerror(errno));
        exit(1);
    }

    new_ut->uctx.uc_stack.ss_sp = stack;
    new_ut->uctx.uc_stack.ss_size = STACK_SIZE - sizeof(uthread_struct);
    new_ut->uctx.uc_link = NULL;
    makecontext(&new_ut->uctx, start_thread, 0);
    new_ut->start_routine = thread_func;
    new_ut->arg = arg;
    new_ut->alive = 1;
    uthreads[uthread_count] = new_ut;
    uthread_count++;
    *ut = new_ut;
}

void uthread_join(uthread_t ut) {
    while(ut->alive) {
        scheduler();
    }
}

void uthread_scheduler(int sig, siginfo_t* si, void* u) {
    printf("uthread_scheduler called\n");
    scheduler();
}

void myFunc(void *arg) {
    int i;
    char* myarg = (char*)arg;
    for (int i = 0; i < 5; ++i) {
        printf("%s[%d %d %d]\n", myarg, gettid(), getppid(), getpid());
        scheduler();
        sleep(2);
    }
}

int main(int argc, char *argv[]) {
    uthread_t ut[3];
    struct sigaction act;

    uthread_struct main_thread;
    uthreads[0] = &main_thread;
    uthread_count = 1;

    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_ONSTACK | SA_SIGINFO;
    act.sa_sigaction = uthread_scheduler;
    sigaction(SIGINT, &act, NULL);

    uthread_create(&ut[0], myFunc, (void*)("thread1"));
    uthread_create(&ut[1], myFunc, (void*)("thread2"));
    uthread_create(&ut[2], myFunc, (void*)("thread3"));

    for (int i = 0; i < 3; ++i) {
        printf("Main thread[%d %d %d]\n", getpid(), getppid(), gettid());
        scheduler();
        sleep(2);
    }

    for (int i = 0; i < 3; ++i) {
        uthread_join(ut[i]);
    }

    printf("All thread have completed!\n");
    return 0;
}
