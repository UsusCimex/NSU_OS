#include <stdio.h>
#include <stdlib.h>

int main() {
    char *env_var_value = getenv("MY_ENV_VAR");
    if (env_var_value) {
        printf("MY_ENV_VAR value: %s\n", env_var_value);
    } else {
        printf("MY_ENV_VAR not found\n");
    }

    int result = setenv("MY_ENV_VAR", "Hello new world", 1);
    if (result != 0) {
        perror("Error changing MY_ENV_VAR value");
        return 1;
    }

    env_var_value = getenv("MY_ENV_VAR");
    if (env_var_value) {
        printf("MY_ENV_VAR new value: %s\n", env_var_value);
    } else {
        printf("MY_ENV_VAR not found\n");
    }

    return 0;
}
