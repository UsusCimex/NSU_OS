#include <stdio.h>

int* create_local_var() {
    int local_var = 42;
    printf("Local variable address: %p\n", &local_var);
    printf("Local variable value: %d\n", local_var);
    void* p = &local_var;
    return p;
}

int main() {
    int* local_var_address = create_local_var();
    printf("\n");
    printf("Local variable address: %p\n", local_var_address);
    printf("Local variable value: %d\n", *local_var_address);
    return 0;
}