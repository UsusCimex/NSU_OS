#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define PORT 1234

int main() {
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t adr_sz;

    struct sockaddr_in serv_adr, from_adr;

    sock = socket(PF_INET, SOCK_DGRAM, 0); // PF_INET - IPv4, SOCK_DGRAM - UDP
    if (sock == -1)
        perror("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET; // IPv4
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(PORT);

    while (1) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        
        if (!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
            break;

        sendto(sock, message, strlen(message), 0,
               (struct sockaddr*)&serv_adr, sizeof(serv_adr));
        adr_sz = sizeof(from_adr);
        str_len = recvfrom(sock, message, BUF_SIZE, 0,
                           (struct sockaddr*)&from_adr, &adr_sz);

        message[str_len] = 0;
        printf("Message from server: %s", message);
    }

    close(sock);
    return 0;
}
