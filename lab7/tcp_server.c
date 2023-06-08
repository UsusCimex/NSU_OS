#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define PORT 4321

int main(int argc, char *argv[]) {
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_adr;
    struct sockaddr_in clnt_adr;
    socklen_t clnt_adr_sz;

    char message[BUF_SIZE];
    int str_len, i;

    pid_t pid;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0); // PF_INET - IPv4, SOCK_DGRAM - TCP
    if(serv_sock == -1)
        perror("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET; // IPv4
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(PORT);

    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        perror("bind() error");

    if(listen(serv_sock, 3) == -1)
        perror("listen() error");

    while(1) {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        if(clnt_sock == -1)
            perror("accept() error");

        pid = fork();
        if(pid == 0) {
            close(serv_sock);
            while((str_len = read(clnt_sock, message, BUF_SIZE)) != 0)
                write(clnt_sock, message, str_len);

            close(clnt_sock);
            exit(0);
        } else {
            close(clnt_sock);
        }
    }

    close(serv_sock);
    return 0;
}
