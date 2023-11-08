#define _GNU_SOURCE
#include <stdatomic.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <linux/futex.h>
#include <syscall.h>

#include <pthread.h>

typedef struct {
    atomic_flag lock;
} spinlock_t;

int futex(atomic_flag *uaddr, int futex_op, int val) {
    return syscall(SYS_futex, uaddr, futex_op, val, NULL, NULL, 0);
}

void spinlock_init(spinlock_t *s) {
    atomic_flag_clear(&s->lock);
}

void spinlock_lock(spinlock_t *s) {
    while (atomic_flag_test_and_set(&s->lock)) {
        int err = futex(&s->lock, FUTEX_WAIT, 0);
        if (err == -1 && errno != EAGAIN) {
            printf("futex FUTEX_WAIT failed: %s\n", strerror(errno));
            abort();
        }
    }
}

void spinlock_unlock(spinlock_t *s) {
    if (atomic_flag_test_and_set(&s->lock)) {
        atomic_flag_clear(&s->lock);

        int err = futex(&s->lock, FUTEX_WAKE, 1);
        if (err == -1) {
            printf("futex FUTEX_WAKE failed: %s\n", strerror(errno));
            abort();
        }
    }
}