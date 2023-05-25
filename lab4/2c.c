#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>

void recursive_function(int i) {
    char stack_array[4096];
    memset(stack_array, 0, 4096);

    if (i % 10 == 0) {
        sleep(1);
    }
    printf("Depth: %d\n", i);
    recursive_function(i + 1);
}

void sigsegv_handler() {
    printf("SIGSEGV caught\n");
    exit(1);
}

int main() {
    printf("PID: %d\n", getpid());
    printf("START: ");
    getchar();

    // STACK OVERFLOW
    // printf("Recursive function\n");
    // recursive_function(1);

    // HEAP OVERFLOW
    // {
    //     printf("Allocating memory on heap\n");
    //     size_t buffer_size = 4096;

    //     void *ptr_array[100];
    //     for (int i = 0; i < 100; i++) {
    //         sleep(1);
    //         ptr_array[i] = malloc(buffer_size);
    //         printf("Heap memory allocated at: %p\n", ptr_array[i]);
    //     }

    //     for(int i = 0; i < 100; i++) {
    //         printf("Freeing allocated memory\n");
    //         free(ptr_array[i]);
    //     }
    // }

    // MMAP
    // while(1) {
    //     printf("Mapping anonymous memory region\n");
    //     size_t region_size = 10 * sysconf(_SC_PAGESIZE);
    //     void *region = mmap(NULL, region_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    //     printf("Mapped anonymous memory region at: %p\n", region);
    // }

    // INTERCEPTION
    // signal(SIGSEGV, sigsegv_handler);

    // CHANGE REGION PROTECTION
    // void *region = mmap(NULL, 10 * sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    // printf("Mapped anonymous memory region at: %p\n", region);
    // mprotect(region, 10 * sysconf(_SC_PAGE_SIZE), PROT_NONE);
    // printf("Changing region protection\n");

    // sleep(5); //time to dead...

    // Attempt one(read):
    // printf("Attempting to read from the region\n");
    // char read_value = *((char*)region);

    // Attempt two(write):
    // printf("Attempting to write to the region\n");
    // *((char*)region) = 1;

    // UNMAPPING
    // printf("Unmapping pages 4-6\n");
    // void *region = mmap(NULL, 10 * sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    // void *unmap_start = (void *)((size_t)region + 4 * sysconf(_SC_PAGESIZE));
    // getchar();
    // munmap(unmap_start, 3 * sysconf(_SC_PAGESIZE));

    printf("END: ");
    getchar();

    return 0;
}