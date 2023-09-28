#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <prog> <args...>\n", argv[0]);
        exit(1);
    }

    pid_t child = fork();

    if (child == 0) {
        // Child proc
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execvp(argv[1], argv + 1);
        perror("execl");
        return 1;
    }
    else {
        // Parent proc
        int status;
        struct user_regs_struct regs;

        while (1) {
            wait(&status);
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                break;
            }

            ptrace(PTRACE_GETREGS, child, NULL, &regs);

            long syscall = regs.orig_rax;
            printf("Syscall: %ld\n", syscall);

            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }

    return 0;
}
