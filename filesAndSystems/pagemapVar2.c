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
    char maps_path[256], pagemap_path[256];
    if (strcmp(argv[1], "self") != 0) {
        pid = atoi(argv[1]);
        snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);
        snprintf(pagemap_path, sizeof(pagemap_path), "/proc/%d/pagemap", pid);
    } else {
        snprintf(maps_path, sizeof(maps_path), "/proc/self/maps");
        snprintf(pagemap_path, sizeof(pagemap_path), "/proc/self/pagemap");
        pid = getpid();
    }

    FILE *maps_file = fopen(maps_path, "r");
    if (!maps_file) {
        fprintf(stderr, "Error opening %s: %s\n", maps_path, strerror(errno));
        return 1;
    }

    int pagemap_fd = open(pagemap_path, O_RDONLY);
    if (pagemap_fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", pagemap_path, strerror(errno));
        fclose(maps_file);
        return 1;
    }

    uintptr_t start, end;
    char line[256];
    while (fgets(line, sizeof(line), maps_file)) {
        sscanf(line, "%lx-%lx", &start, &end);
        off_t pagemap_offset = (start / sysconf(_SC_PAGE_SIZE)) * sizeof(uint64_t);

        if (lseek(pagemap_fd, pagemap_offset, SEEK_SET) != pagemap_offset) {
            fprintf(stderr, "Error seeking in %s: %s\n", pagemap_path, strerror(errno));
            fclose(maps_file);
            close(pagemap_fd);
            return 1;
        }

        for (uintptr_t va = start; va < end; va += sysconf(_SC_PAGE_SIZE)) {
            uint64_t entry;
            if (read(pagemap_fd, &entry, sizeof(entry)) != sizeof(entry)) {
                fprintf(stderr, "Error reading %s: %s\n", pagemap_path, strerror(errno));
                fclose(maps_file);
                close(pagemap_fd);
                return 1;
            }

            if (entry & (1ULL << 63)) {
                uint64_t pfn = entry & PFN_MASK;
                uintptr_t pa = pfn << 12;
                printf("VA: 0x%016lx -> PA: 0x%016lx\n", va, pa);
            }
        }
    }

    fclose(maps_file);
    close(pagemap_fd);
    return 0;
}
