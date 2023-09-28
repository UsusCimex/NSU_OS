#include <stdio.h>
#include <sys/syscall.h>

int main() {
    // printf("hello!\n");
    // write(1, "hello!\n", 7);
    // syscall(SYS_write, 0, "hello!\n", 7);
    // syscall(SYS_exit, 0);

    printHello(1, "hello\n", 6);
}