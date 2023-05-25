#include <stdio.h>
#include <unistd.h>

// глобальная инициализированная переменная
int global_initialized_var = 42;

// глобальная неинициализированная переменная
int global_uninitialized_var;

// глобальная константа
const int global_const = 100;

void print_var_addresses() {
    // локальная переменная
    int local_var = 10;

    // статическая локальная переменная
    static int static_local_var = 20;

    // константа в функции
    const int const_local_var = 30;

    printf("Address of local_var: %p\n", &local_var);
    printf("Address of static_local_var: %p\n", &static_local_var);
    printf("Address of const_local_var: %p\n", &const_local_var);
}

int main() {
    printf("Address of global_initialized_var: %p\n", &global_initialized_var);
    printf("Address of global_uninitialized_var: %p\n", &global_uninitialized_var);
    printf("Address of global_const: %p\n", &global_const);

    print_var_addresses();
    printf("%d\n", getpid());
    getchar();

    return 0;
}