#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 80
#define BUFFER_SIZE 4096

void* handle_connection(void* client_socket);
int connect_to_target(char* host, int port);

int main(int argc, char* argv[]) {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addr_len = sizeof(address);
    int opt = 1;

    // Создание сокета сервера
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Настройка сокета для повторного использования порта
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Адрес сервера
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязка сокета к порту 80
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Начало прослушивания
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("HTTP Proxy Server Listening on port %d\n", PORT);

    while (1) {
        // Принятие соединения от клиента
        if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addr_len)) < 0) {
            perror("accept");
            continue;
        }

        // Создание потока для обработки соединения
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_connection, (void*)&client_socket) != 0) {
            perror("pthread_create");
            continue;
        }
    }

    return 0;
}

void* handle_connection(void* client_socket) {
    int client_sock = *(int*)client_socket;
    char buffer[BUFFER_SIZE];
    int target_sock, read_bytes;

    // Чтение запроса от клиента
    read_bytes = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (read_bytes <= 0) {
        close(client_sock);
        return NULL;
    }
    buffer[read_bytes] = '\0'; // Добавляем нуль-терминатор

    // Извлечение имени хоста из запроса
    char host[_SC_HOST_NAME_MAX + 1] = {0};
    char* host_line_start = strstr(buffer, "Host: ");
    if (!host_line_start) {
        host_line_start = strstr(buffer, "host: "); // Попытка найти заголовок в другом регистре
    }

    if (host_line_start) {
        char* host_line_end = strstr(host_line_start, "\r\n");
        if (!host_line_end) {
            host_line_end = strstr(host_line_start, "\n");
        }

        if (host_line_end) {
            int host_length = host_line_end - (host_line_start + 6);
            strncpy(host, host_line_start + 6, host_length);
            host[host_length] = '\0'; // Обеспечиваем правильное завершение строки
        } else {
            printf("Host header line end not found.\n");
        }
    } else {
        printf("Host header not found in request.\n");
        close(client_sock);
        return NULL;
    }

    printf("Connecting to host: %s\n", host);

    // Подключение к целевому серверу
    target_sock = connect_to_target(host, PORT); // Используем PORT, определенный в начале программы
    if (target_sock < 0) {
        close(client_sock);
        return NULL;
    }

    // Передача запроса на целевой сервер
    send(target_sock, buffer, strlen(buffer), 0);

    // Передача ответа обратно клиенту
    while ((read_bytes = recv(target_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        send(client_sock, buffer, read_bytes, 0);
    }

    // Закрытие соединений
    close(target_sock);
    close(client_sock);
    return NULL;
}

int connect_to_target(char* host, int port) {
    struct hostent *he;
    struct sockaddr_in server_addr;
    int sock;

    // Получение информации о хосте
    if ((he = gethostbyname(host)) == NULL) {
        herror("gethostbyname failed");
        return -1;
    }

    // Создание сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    // Настройка параметров сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    return sock;
}