#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/syscall.h>

int main(int argc, char **argv) {
    pid_t child;
    long orig_eax;
    int status;
    int syscall_num;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s program [args ...]\n", argv[0]);
        return 1;
    }

    child = fork();
    if (child == 0) {
        /* Child process */
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], argv + 1);
        perror("execvp");
        exit(1);
    } else {
        /* Parent process */
        wait(&status);
        while (WIFSTOPPED(status)) {
            /* Get the system call number */
            orig_eax = ptrace(PTRACE_PEEKUSER, child, sizeof(long) * ORIG_RAX, NULL);
            syscall_num = (int)orig_eax;

            /* Print the system call number */
            printf("syscall %d\n", syscall_num);

            /* Continue execution */
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            wait(&status);
        }
    }

    return 0;
}