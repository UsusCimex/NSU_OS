#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

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

                char fullNameBuffer[1024];
                fullPath(oldPath, oldName, fullNameBuffer);

                FILE *oldFile = fopen(fullNameBuffer, "rb");
                if (oldFile == NULL) {
                    printf("|%s|\n", fullNameBuffer);
                    perror("fopen failed");
                    continue;
                }

                fullPath(newPath, newName, fullNameBuffer);

                FILE *newFile = fopen(fullNameBuffer, "wb");
                if (newFile == NULL) {
                    printf("|%s|\n", fullNameBuffer);
                    perror("fopen failed");
                    fclose(oldFile);
                    continue;
                }

                char buffer[1024];
                char reversedBuffer[1024];
                size_t readLen;
                while ((readLen = fread(buffer, 1, sizeof(buffer), oldFile)) > 0) {
                    for (int i = 0; i < readLen; ++i) {
                        reversedBuffer[i] = buffer[readLen - i - 1];
                    }
                    fwrite(reversedBuffer, 1, readLen, newFile);
                }

                fclose(oldFile);
                fclose(newFile);
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
        return 1;
    }
    closedir(dir);
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2 || checkDir(argv[1])) {
        printf("Usage : ./lab3 <directory>\n");
        return 1;
    }
    
    reverseDirectory(argv[1]);
    return 0;
}
