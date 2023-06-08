#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define PORT 4321

int main(int argc, char *argv[]) {
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_adr;

    sock = socket(PF_INET, SOCK_STREAM, 0); // PF_INET - IPv4, SOCK_DGRAM - TCP
    if(sock == -1)
        perror("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET; // IPv4
    serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_adr.sin_port = htons(PORT);

    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        perror("connect() error");

    while(1) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
            break;

        write(sock, message, strlen(message));
        str_len = read(sock, message, BUF_SIZE-1);
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }

    close(sock);
    return 0;
}
