#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void memory_operations() {
    char *buffer1 = (char*)malloc(100);
    strcpy(buffer1, "hello world");
    printf("Buffer1 content: %s\n", buffer1);
    free(buffer1);
    // strcpy(buffer1, "deleted message");
    printf("Buffer1 content after free: %s\n", buffer1);
    getchar();

    char *buffer2 = (char*)malloc(100);
    strcpy(buffer2, "hello world");
    printf("Buffer2 content: %s\n", buffer2);
    char* middle_ptr = &buffer2[50];
    free(middle_ptr);
    printf("Buffer2 content: %s\n", buffer2);
    getchar();
}

int main() {
    memory_operations();
    return 0;
}