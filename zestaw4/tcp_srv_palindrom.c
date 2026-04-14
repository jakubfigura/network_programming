#define _GNU_SOURCE
#define PORT 2020
#define RESPONSE_SIZE 32
#define MAX_CLIENTS 200
#define LINE_LIMIT 1026
#define MAX_EVENTS 16
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/epoll.h>


// bufor dla klienta 
struct  client
{
    int fd;     // deskryptor klienta
    char buf[LINE_LIMIT];   // bufor na dane klineta 
    int buf_len; 
};

// globalna tablica klientow 
struct client clients[MAX_CLIENTS];


bool is_ascii_letter(unsigned char c){
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int to_lower(int c){

    if(c >= 'A' && c <= 'Z'){
        return c + 32;
    }

    return c;
}

bool validate(unsigned char *buf, ssize_t cnt){
    /*funkcja sprawdzająca poprawność danych w buforze*/

    /*akceptacja pustego komunikatu*/
    if (cnt == 0) {
        return true;
    }

    if(buf[0] == ' ' || buf[cnt - 1] == ' '){
        return false;
    }

    
    for(int i = 0; i < cnt; i++){
        /* wykluczenie powtarzających spacji*/
        if(buf[i] == ' ' && i > 0 && i < cnt - 1){
            if(buf[i+1] == ' '){
                return false;
            }
        }
        /*sprawdzenie, czy znaki to litery a-z[A-Z] lub spacja*/
        // zakres ASCII 
        if(!is_ascii_letter(buf[i]) && buf[i] != ' '){
            return false;
        }
    }
    return true;
}

bool is_palindrom(unsigned char *word, int len){
    /* sprawdzamy, czy wyraz jest palindromem, ignorowana jest wielkość liter poprzez tolower*/
    for(int i = 0, j = len - 1; i < j; i++, j--){
        //TODO
        if(to_lower(word[i]) != to_lower(word[j])){
            return false;
        }
    }
    return true;
}


int main(void)
{
    // inicjalizacja tablicy klientow

    for (int i = 0; i < MAX_CLIENTS; i++){
        clients[i].fd = -1; 
    }

    int srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_sock == -1){
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET, 
        .sin_addr.s_addr = htonl(INADDR_ANY), 
        .sin_port = htons(PORT)
    };

    if (bind(srv_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1){
        perror("bind"); 
        return 1;
    }

    if (listen(srv_sock, 10) == -1){
        perror("listen");
        return 1;
    }

    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1){
        perror("epoll_create");
        return 1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = srv_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, srv_sock, &ev) == -1){
        perror("epoll_ctl");
        return 1;
    }

    struct epoll_event events[MAX_EVENTS];

    while(true){
        int num = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num == -1){
            perror("epoll_wait");
            break;
        }

        for(int i = 0; i < num; i++){
            int fd = events[i].data.fd;

            if(fd == srv_sock){
                int cli_fd = accept(srv_sock, NULL, NULL);
                if (cli_fd == -1){
                    perror("accept");
                    continue;
                }

                int slot = -1;
                for (int j = 0; j < MAX_CLIENTS; j++){
                    if(clients[j].fd == -1){
                        slot = j; 
                        break;
                    }
                }

                if(slot == -1){
                    // gdy brak miejsc to odrzucamy klienta 
                    close(cli_fd);
                    continue;
                }

                clients[slot].fd = cli_fd;
                clients[slot].buf_len = 0;

                ev.events = EPOLLIN;
                ev.data.fd = cli_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cli_fd, &ev) == -1) {
                    perror("epoll_ctl");
                    close(cli_fd);
                    clients[slot].fd = -1;
                    continue;
                }

            }
            else{
                int slot = -1;
                for (int j = 0; j < MAX_CLIENTS; j++){
                    if(clients[j].fd == fd){ slot = j; break; }
                }
                if (slot == -1) continue;

                struct client *c = &clients[slot];

                ssize_t n = read(fd, c->buf + c->buf_len, LINE_LIMIT - c->buf_len);
                if(n <= 0){
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    c->fd = -1;
                    c->buf_len = 0;
                    continue;
                }

                c->buf_len += n;

                if (c->buf_len == LINE_LIMIT) {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    c->fd = -1;
                    c->buf_len = 0;
                    continue;
                }

                for (int j = 0; j < c->buf_len; j++) {
                    if (c->buf[j] == '\n') {
                        int line_len = j;
                        if (line_len > 0 && c->buf[line_len - 1] == '\r') {
                            line_len--;
                        }

                        if (!validate((unsigned char *)c->buf, line_len)) {
                            write(fd, "ERROR\r\n", 7);
                        } else {
                            int total = 0;
                            int palindromes = 0;
                            int word_start = 0;

                            for (int k = 0; k <= line_len; k++) {
                                if (k == line_len || c->buf[k] == ' ') {
                                    int word_len = k - word_start;
                                    if (word_len > 0) {
                                        total++;
                                        if (is_palindrom((unsigned char *)c->buf + word_start, word_len)) {
                                            palindromes++;
                                        }
                                    }
                                    word_start = k + 1;
                                }
                            }

                            char response[RESPONSE_SIZE];
                            int response_len = snprintf(response, sizeof(response), "%d/%d\r\n", palindromes, total);
                            write(fd, response, response_len);
                        }

                        int consumed = j + 1;
                        memmove(c->buf, c->buf + consumed, c->buf_len - consumed);
                        c->buf_len -= consumed;
                        j = -1;
                    }
                }

            }
        }
    }

    return 0;
}
