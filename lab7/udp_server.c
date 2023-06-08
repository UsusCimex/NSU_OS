#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define PORT 1234

int main() {
    int serv_sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t clnt_adr_sz;

    struct sockaddr_in serv_adr, clnt_adr;

    serv_sock = socket(PF_INET, SOCK_DGRAM, 0); // PF_INET - IPv4, SOCK_DGRAM - UDP
    if (serv_sock == -1)
        perror("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET; // IPv4
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        perror("bind() error");

    while (1) {
        clnt_adr_sz = sizeof(clnt_adr);
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0,
                           (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        // printf("%s", message);
        sendto(serv_sock, message, str_len, 0,
               (struct sockaddr*)&clnt_adr, clnt_adr_sz);
    }

    close(serv_sock);
    return 0;
}
