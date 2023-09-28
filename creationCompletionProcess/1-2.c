#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global_var = 0; // Глобальная переменная

int main() {
    int local_var = 0; // Локальная переменная

    printf("Адрес глобальной переменной: %p\n", &global_var);
    printf("Адрес локальной переменной: %p\n", &local_var);
    printf("PID: %d\n", getpid());

    pid_t pid = fork(); // Создание нового процесса

    if (pid == 0) { // Дочерний процесс
        printf("Child PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());
        printf("Адрес глобальной переменной в дочернем процессе: %p\n", &global_var);
        printf("Адрес локальной переменной в дочернем процессе: %p\n", &local_var);

        global_var = 5;
        local_var = 5;

        printf("Значение глобальной переменной в дочернем процессе: %d\n", global_var);
        printf("Значение локальной переменной в дочернем процессе: %d\n", local_var);

        sleep(40); /// необходимо для 2 задания

        exit(5);
    } else if (pid > 0) { // Родительский процесс
        printf("Значение глобальной переменной в родительском процессе: %d\n", global_var);
        printf("Значение локальной переменной в родительском процессе: %d\n", local_var);

        sleep(10);
        exit(5); /// необходимо для 2 задания

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился с кодом: %d\n", WEXITSTATUS(status));
        } else {
            printf("Дочерний процесс завершился некорректно.\n");
        }
    } else {
        printf("Ошибка при создании дочернего процесса.\n");
        return 1;
    }

    return 0;
}
