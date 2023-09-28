#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("PID: %d\n", getpid());

    getchar();

    if (argc < 2) {
        char *new_argv[] = {argv[0], "restarted", NULL};
        execv(argv[0], new_argv);
    }

    printf("Hello world\n");
    getchar();
    return 0;
}
