#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void reverseText(char* text, int from, int to) {
    if (to <= from) return;
    char* temp = (char*)malloc((to - from + 1) * sizeof(char));
    for (int i = from; i <= to; ++i) {
        temp[i - from] = text[to - i + from];
    }
    for (int i = from; i <= to; ++i) {
        text[i] = temp[i - from];
    }
    free(temp);
}

void getReversedPath(char* oldPath, char* newPath) {
    int len = strlen(oldPath);
    strcpy(newPath, oldPath);
    int tempPointer = 0;
    for (int i = 0; i < len; ++i) {
        if (newPath[i] == '/') {
            reverseText(newPath, tempPointer, i - 1);
            tempPointer = i + 1;
        }
    }
    reverseText(newPath, tempPointer, len - 1);
}

void createNewPath(char* path) {
    int len = strlen(path);
    char* tempPath = (char*)malloc(len * sizeof(char));
    for (int i = 0; i < len; ++i) {
        if (path[i] == '/') {
            mkdir(tempPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        tempPath[i] = path[i];
    }
    mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void fullPath(char* path, char* fileName, char* result) {
    for (int i = 0; i < strlen(path); ++i) {
        result[i] = path[i];
    }
    for (int i = strlen(path); i < strlen(path) + strlen(fileName); ++i) {
        result[i] = fileName[i - strlen(path)];
    }
    result[strlen(path) + strlen(fileName)] = '\0';
}

void createNewFiles(char* oldPath, char* newPath) {
    DIR *dir;
    struct dirent *ent;

    dir = opendir(oldPath);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {  // если это обычный файл
                char *oldName = ent->d_name;
                int len = strlen(oldName);
                char newName[len + 1];
                for (int i = 0; i < len; i++) {
                    newName[i] = oldName[len - i - 1];
                }
                newName[len] = '\0';

                char fullNameBuffer[FILENAME_MAX];
                fullPath(oldPath, oldName, fullNameBuffer);

                int fdOld = open(fullNameBuffer, O_RDONLY);
                if (fdOld == -1) {
                    perror("open input file");
                    exit(1);
                }

                fullPath(newPath, newName, fullNameBuffer);

                int fdNew = open(fullNameBuffer, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                if (fdNew == -1) {
                    perror("open output file");
                    exit(1);
                }

                struct stat st;
                if (fstat(fdOld, &st) == -1) {
                    perror("fstat");
                    exit(1);
                }
                off_t file_size = st.st_size;

                if (lseek(fdOld, 0, SEEK_END) == -1) {
                    perror("lseek");
                    exit(1);
                }

                off_t pos = file_size;
                int block_size = 1024;
                char* buffer = (char*)malloc(block_size * sizeof(char));
                while (pos > 0) {
                    pos -= block_size;
                    if (pos < 0) {
                        block_size += pos;
                        pos = 0;
                    }
                    if (lseek(fdOld, pos, SEEK_SET) == -1) {
                        perror("lseek");
                        exit(1);
                    }
                    ssize_t bytes_read = read(fdOld, buffer, block_size);
                    if (bytes_read == -1) {
                        perror("read");
                        exit(1);
                    }
                    for (int i = 0; i < bytes_read; i++) {
                        if (write(fdNew, &buffer[bytes_read - i - 1], 1) == -1) {
                            perror("write");
                            exit(1);
                        }
                    }
                }
                free(buffer);

                if (close(fdOld) == -1) {
                    perror("close input file");
                    exit(1);
                }
                if (close(fdNew) == -1) {
                    perror("close output file");
                    exit(1);
                }
            }
        }
        closedir(dir);
    } else {
        perror("opendir failed");
    }
}

void reverseDirectory(char* path) {
    char* newPath = (char*)malloc(strlen(path) * sizeof(char));
    getReversedPath(path, newPath);

    createNewPath(newPath);
    createNewFiles(path, newPath);
}

int checkDir(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        printf("Error open directory: %s\n", path);
        exit(1);
    }
    closedir(dir);
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2 || checkDir(argv[1])) {
        printf("Usage : ./lab3 <directory>\n");
        exit(1);
    }
    
    reverseDirectory(argv[1]);
    return 0;
}
