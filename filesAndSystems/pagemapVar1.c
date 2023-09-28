#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

#define PFN_MASK ((((uint64_t)1) << 55) - 1)

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    pid_t pid;
    char pagemap_path[256];
    if (strcmp(argv[1], "self") != 0) {
        pid = atoi(argv[1]);
        snprintf(pagemap_path, sizeof(pagemap_path), "/proc/%d/pagemap", pid);
    } else {
        snprintf(pagemap_path, sizeof(pagemap_path), "/proc/self/pagemap");
        pid = getpid();
    }

    int fd = open(pagemap_path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", pagemap_path, strerror(errno));
        return 1;
    }

    printf("Reading pagemap file for PID %d: %s\n", pid, pagemap_path);

    uint64_t entry;
    size_t read_bytes;
    uintptr_t virtual_address = 0;

    while ((read_bytes = read(fd, &entry, sizeof(entry))) > 0) {
        if (entry & (1ULL << 63)) {
            uint64_t pfn = entry & PFN_MASK;
            uintptr_t physical_address = pfn << 12;
            printf("VA: 0x%016lx -> PA: 0x%016lx\n", virtual_address, physical_address);
        }
        virtual_address += sysconf(_SC_PAGE_SIZE);
    }

    if (read_bytes < 0) {
        fprintf(stderr, "Error reading %s: %s\n", pagemap_path, strerror(errno));
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
