// Szkielet serwera UDP/IPv4 używającego gniazdka bezpołączeniowego.

#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
    int sock;
    int rc;         // "rc" to skrót słów "result code"
    ssize_t cnt;    // na wyniki zwracane przez recvfrom() i sendto()
    int port;

    if(argc != 2){
        fprintf(stderr, "Podaj argumenty: <nazwa_programu> <port>\n");
        return 1;
    }

    port = atoi(argv[1]);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }


    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = htonl(INADDR_ANY) },
        .sin_port = htons(port)
    };

    rc = bind(sock, (struct sockaddr *) & addr, sizeof(addr));
    if (rc == -1) {
        perror("bind");
        return 1;
    }

    bool keep_on_handling_clients = true;
    while (keep_on_handling_clients) {

        unsigned char buf[16];
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_len;

        clnt_addr_len = sizeof(clnt_addr);
        cnt = recvfrom(sock, buf, 16, 0,
                (struct sockaddr *) & clnt_addr, & clnt_addr_len);
        if (cnt == -1) {
            perror("recvfrom");
            return 1;
        }
        printf("received %zi bytes\n", cnt);

        memcpy(buf, "Hello, world!\r\n", 15);

        cnt = sendto(sock, buf, 15, 0,
                (struct sockaddr *) & clnt_addr, clnt_addr_len);
        if (cnt == -1) {
            perror("sendto");
            return 1;
        }
        printf("sent %zi bytes\n", cnt);

    }

    rc = close(sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;
}
