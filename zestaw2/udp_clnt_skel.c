// Szkielet klienta UDP/IPv4 używającego gniazdka bezpołączeniowego.

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>


bool is_printable_char(char ch){
    if(ch < 32 || ch > 126){
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    int sock;
    int rc;         // "rc" to skrót słów "result code"
    ssize_t cnt;    // na wyniki zwracane przez recvfrom() i sendto()
    int port;

    if(argc != 3){
        fprintf(stderr, "Podaj argumety: <nazwa_programu> <adres_ip> <port>\n");
        return 1;
    }

    port = atoi(argv[2]);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = inet_addr(argv[1]) },   // 127.0.0.1
        .sin_port = htons(port)
    };

    unsigned char buf[16];

    cnt = sendto(sock, NULL, 0, 0, (struct sockaddr *) &addr, sizeof (addr));
    if (cnt == -1) {
        perror("sendto");
        return 1;
    }
    printf("sent %zi bytes\n", cnt);

    cnt = recvfrom(sock, buf, 16, 0, NULL, NULL);
    if (cnt == -1) {
        perror("recvfrom");
        return 1;
    }

    for(int i = 0; i < cnt; i++){
        if(is_printable_char(buf[i]) || buf[i] == '\n' || buf[i] == '\r' || buf[i] == '\t'){
            putchar(buf[i]);
        }
    }
    printf("received %zi bytes\n", cnt);

    rc = close(sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;
}
