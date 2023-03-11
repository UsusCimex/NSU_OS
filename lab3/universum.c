#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>

char* modeToString(mode_t mode);
mode_t stringToMode(const char* modeStr);

// Создание каталога //
void createDirectory(const char *path)
{
    if (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
    {
        perror("mkdir failed");
        exit(EXIT_FAILURE);
    }
}

// Вывод содержимого каталога //
void listDirectory(const char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir failed");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        printf("%s\n", entry->d_name);
    }

    if (closedir(dir) == -1)
    {
        perror("closedir failed");
        exit(EXIT_FAILURE);
    }
}

// Удаление каталога //
void deleteDirectory(const char *path)
{
    if (rmdir(path) == -1)
    {
        perror("rmdir failed");
        exit(EXIT_FAILURE);
    }
}

// Создание файла //
void createFile(const char *path)
{
    int fd = open(path, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1)
    {
        perror("close failed");
        exit(EXIT_FAILURE);
    }
}

// Чтение из файла //
void readFile(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    char buffer[PATH_MAX];
    ssize_t n_read;
    while ((n_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (write(STDOUT_FILENO, buffer, n_read) == -1)
        {
            perror("write failed");
            exit(EXIT_FAILURE);
        }
    }

    if (close(fd) == -1)
    {
        perror("close failed");
        exit(EXIT_FAILURE);
    }
}

// Удаление файла //
void deleteFile(const char *path)
{
    if (remove(path) == -1)
    {
        perror("remove failed");
        exit(EXIT_FAILURE);
    }
}

// Создание символьной ссылки //
void createSymbolicLink(const char *path, const char *target)
{
    if (symlink(target, path) == -1)
    {
        perror("symlink failed");
        exit(EXIT_FAILURE);
    }
}

// Вывод содержимого символьной ссылки //
void readSymbolicLink(const char *path)
{
    char buffer[PATH_MAX];
    ssize_t n_read = readlink(path, buffer, sizeof(buffer));
    if (n_read == -1)
    {
        perror("readlink failed");
        exit(EXIT_FAILURE);
    }

    buffer[n_read] = '\0';
    printf("%s\n", buffer);
}

// Вывод содержимого файла, на который указывает символьная ссылка //
void readLinkedFile(const char *path)
{
    char buffer[PATH_MAX];
    ssize_t n_read = readlink(path, buffer, sizeof(buffer));
    if (n_read == -1)
    {
        perror("readlink failed");
        exit(EXIT_FAILURE);
    }

    buffer[n_read] = '\0';
    readFile(buffer);
}   

// удалить символьную ссылку на файл //
void deleteSymbolicLink(const char *path)
{
    if (unlink(path) == -1)
    {
        perror("unlink failed");
        exit(EXIT_FAILURE);
    }
}

// создать жесткую ссылку на файл //
void createHardLink(const char *path, const char *target)
{
    if (link(target, path) == -1)
    {
        perror("link failed");
        exit(EXIT_FAILURE);
    }
}

// удалить жесткую ссылку на файл //
void deleteHardLink(const char *path)
{
    if (unlink(path) == -1)
    {
        perror("Error deleting hard link");
        exit(EXIT_FAILURE);
    }
}

// вывод прав доступа к файлу и количество жестких ссылок //
void printFileInfo(const char *path)
{
    struct stat sb;
    if (stat(path, &sb) == -1)
    {
        perror("Error getting file info");
        exit(EXIT_FAILURE);
    }

    char* mode = modeToString(sb.st_mode);
    printf("File permissions: %s\n", mode);
    printf("Number of hard links: %lu\n", sb.st_nlink);

    free(mode);
}

// изменение прав доступа к файлу //
void changeFilePermissions(const char *path, mode_t newPerms)
{
    if (chmod(path, newPerms) == -1)
    {
        perror("Error changing file permissions");
        exit(EXIT_FAILURE);
    }
}

// Change from char* to mode_t
mode_t stringToMode(const char* modeStr)
{
    mode_t mode = 0;
    int i;

    for (i = 0; i < strlen(modeStr); i++) {
        if (modeStr[i] == 'r') {
            mode |= S_IRUSR | S_IRGRP | S_IROTH;
        } else if (modeStr[i] == 'w') {
            mode |= S_IWUSR | S_IWGRP | S_IWOTH;
        } else if (modeStr[i] == 'x') {
            mode |= S_IXUSR | S_IXGRP | S_IXOTH;
        } else {
            fprintf(stderr, "Error: Invalid mode character '%c'\n", modeStr[i]);
            exit(EXIT_FAILURE);
        }
    }

    return mode;
}

char* modeToString(mode_t mode) {
    char* result = (char*)malloc(sizeof(char) * 10);
    if (result == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int i;
    for (i = 0; i < 9; ++i) {
        result[i] = '-';
    }
    result[9] = '\0';

    if (S_ISDIR(mode)) {
        result[0] = 'd';
    }
    else if (S_ISLNK(mode)) {
        result[0] = 'l';
    }

    if ((mode & S_IRUSR) != 0) {
        result[1] = 'r';
    }
    if ((mode & S_IWUSR) != 0) {
        result[2] = 'w';
    }
    if ((mode & S_IXUSR) != 0) {
        result[3] = 'x';
    }
    if ((mode & S_IRGRP) != 0) {
        result[4] = 'r';
    }
    if ((mode & S_IWGRP) != 0) {
        result[5] = 'w';
    }
    if ((mode & S_IXGRP) != 0) {
        result[6] = 'x';
    }
    if ((mode & S_IROTH) != 0) {
        result[7] = 'r';
    }
    if ((mode & S_IWOTH) != 0) {
        result[8] = 'w';
    }
    if ((mode & S_IXOTH) != 0) {
        result[9] = 'x';
    }

    return result;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <operationName> <args>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "createDirectory") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        createDirectory(argv[2]);
    }
    else if (strcmp(argv[1], "listDirectory") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        listDirectory(argv[2]);
    }
    else if (strcmp(argv[1], "deleteDirectory") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        deleteDirectory(argv[2]);
    }
    else if (strcmp(argv[1], "createFile") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        createFile(argv[2]);
    }
    else if (strcmp(argv[1], "readFile") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        readFile(argv[2]);
    }
    else if (strcmp(argv[1], "deleteFile") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        deleteFile(argv[2]);
    }
    else if (strcmp(argv[1], "createSymbolicLink") == 0) { //2 args
        if (argc < 4) {
            printf("This operation use 2 args!\n");
            return 1;
        }
        createSymbolicLink(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "readSymbolicLink") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        readSymbolicLink(argv[2]);
    }
    else if (strcmp(argv[1], "readLinkedFile") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        readLinkedFile(argv[2]);
    }
    else if (strcmp(argv[1], "deleteSymbolicLink") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        deleteSymbolicLink(argv[2]);
    }
    else if (strcmp(argv[1], "createHardLink") == 0) { //2 args
        if (argc < 4) {
            printf("This operation use 2 args!\n");
            return 1;
        }
        createHardLink(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "deleteHardLink") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        deleteHardLink(argv[2]);
    }
    else if (strcmp(argv[1], "printFileInfo") == 0) { //1 arg
        if (argc < 3) {
            printf("This operation use 1 arg!\n");
            return 1;
        }
        printFileInfo(argv[2]);
    }
    else if (strcmp(argv[1], "changeFilePermissions") == 0) { //2 args
        if (argc < 4) {
            printf("This operation use 2 args!\n");
            return 1;
        }
        mode_t mode = stringToMode(argv[3]);
        changeFilePermissions(argv[2], mode);
    }
    else {
        printf("Operation(%s) not found!\n", argv[1]);
        return 1;
    }

    printf("\nOperation completed!\n");
}