#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

// Структура для передачи данных в поток
typedef struct {
    char src[1024];
    char dest[1024];
} dir_data;

void* copy_file(void* arg) {
    dir_data* data = (dir_data*)arg;
    // printf("[%d] Starting to copy: %s to %s\n", gettid(), data->src, data->dest);

    int src_fd = open(data->src, O_RDONLY);
    if (src_fd < 0) {
        perror("Failed to open source file");
        free(data);
        return NULL;
    }

    int dest_fd = open(data->dest, O_WRONLY | O_CREAT, 0644);
    if (dest_fd < 0) {
        perror("Failed to open destination file");
        close(src_fd);
        free(data);
        return NULL;
    }

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        if (write(dest_fd, buffer, bytes_read) != bytes_read) {
            perror("Failed to write to destination file");
            break;
        }
    }

    close(src_fd);
    close(dest_fd);
    free(data);
    return NULL;
}


void* process_directory(void* arg) {
    dir_data* data = (dir_data* )arg;
    DIR* dir = opendir(data->src);
    if (!dir) {
        perror("Failed to open directory");
        free(data);
        return NULL;
    }

    pthread_t threadArr[1024]; //Ограниченное количество файлов в директории (Можно создать динамический массив)
    int threadC = 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Пропуск текущей и родительской директории
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[1024], dest_path[1024];
        sprintf(src_path, "%s/%s", data->src, entry->d_name);
        sprintf(dest_path, "%s/%s", data->dest, entry->d_name);

        struct stat statbuf;
        if (stat(src_path, &statbuf) == 0) {
            if (S_ISREG(statbuf.st_mode)) {
                // printf("[%d] Found file: %s\n", gettid(), src_path);

                dir_data* file_data = (dir_data*)malloc(sizeof(dir_data));
                strcpy(file_data->src, src_path);
                strcpy(file_data->dest, dest_path);
                pthread_create(threadArr + threadC, NULL, copy_file, file_data);
                threadC++;
            } else if (S_ISDIR(statbuf.st_mode)) {
                mkdir(dest_path, 0755);
                dir_data* dirent_data = (dir_data*)malloc(sizeof(dir_data));
                strcpy(dirent_data->src, src_path);
                strcpy(dirent_data->dest, dest_path);
                pthread_create(threadArr + threadC, NULL, process_directory, dirent_data);
                threadC++;
            }
        } else {
            perror("Failed to get file status");
        }
    }

    closedir(dir);
    free(data);

    for (int threadP = 0; threadP < threadC; ++threadP) {
        pthread_join(threadArr[threadP], NULL);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <source_directory> <destination_directory>\n", argv[0]);
        return 1;
    }

    dir_data* data = (dir_data*)malloc(sizeof(dir_data));
    strcpy(data->src, argv[1]);
    strcpy(data->dest, argv[2]);

    mkdir(data->dest, 0755);

    pthread_t thread;
    pthread_create(&thread, NULL, process_directory, data);
    pthread_join(thread, NULL);

    return 0;
}
