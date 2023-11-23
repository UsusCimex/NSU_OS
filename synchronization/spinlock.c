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

void spinlock_init(spinlock_t *s) {
    atomic_flag_clear(&s->lock);
}

void spinlock_lock(spinlock_t *s) {
    while (atomic_flag_test_and_set(&s->lock)) { }
}

void spinlock_unlock(spinlock_t *s) {
    atomic_flag_clear(&s->lock);
}