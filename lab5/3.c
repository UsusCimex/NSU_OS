#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)
#define FILE_PATH "file.txt"

void recursive_func(int depth) {
    if (depth >= 10)
        return;
    char str[] = "hello world";
    recursive_func(depth + 1);
}

int child_func() {
    recursive_func(0);
    return 0;
}

int main() {
    int fd = open(FILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, STACK_SIZE) == -1) {
        perror("ftruncate");
        close(fd);
        exit(EXIT_FAILURE);
    }

    void *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    pid_t pid = clone(child_func, stack + STACK_SIZE, SIGCHLD, NULL);
    if (pid == -1) {
        perror("clone");
        munmap(stack, STACK_SIZE);
        close(fd);
        exit(EXIT_FAILURE);
    }

    int status;
    waitpid(pid, &status, 0);

    munmap(stack, STACK_SIZE);
    close(fd);
    return 0;
}
