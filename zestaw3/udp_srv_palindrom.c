#define _POSIX_C_SOURCE 200809L
#define DATA_SIZE 65507
#define PORT 2020
#define RESPONSE_SIZE 32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <ctype.h>



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
        if(!isalpha(buf[i]) && buf[i] != ' '){
            return false;
        }
    }
    return true;
}

bool is_palindrom(unsigned char *word, int len){
    /* sprawdzamy, czy wyraz jest palindromem, ignorowana jest wielkość liter poprzez tolower*/
    for(int i = 0, j = len - 1; i < j; i++, j--){

        if(tolower(word[i]) != tolower(word[j])){
            return false;
        }
    }
    return true;
}


int main(void)
{
    int sock;
    int rc;         // "rc" to skrót słów "result code"
    ssize_t cnt;    // na wyniki zwracane przez recvfrom() i sendto()

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }


    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = htonl(INADDR_ANY) },
        .sin_port = htons(PORT)
    };

    rc = bind(sock, (struct sockaddr *) & addr, sizeof(addr));
    if (rc == -1) {
        perror("bind");
        return 1;
    }

    bool keep_on_handling_clients = true;
    while (keep_on_handling_clients) {

        unsigned char buf[DATA_SIZE];
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_len;

        clnt_addr_len = sizeof(clnt_addr);
        cnt = recvfrom(sock, buf, DATA_SIZE, 0,
                (struct sockaddr *) & clnt_addr, & clnt_addr_len);
        if (cnt == -1) {
            perror("recvfrom");
            return 1;
        }
        
        /*obcinanie końcowych bajtow*/
        if(cnt > 0 && buf[cnt-1] == '\n'){
            cnt--;
            if (cnt > 0 && buf[cnt-1] == '\r'){
                cnt--;
            }
        }

    
        if(!validate(buf, cnt)){
            sendto(sock, "ERROR", 5, 0, (struct sockaddr *) &clnt_addr, clnt_addr_len);
            continue;
        }

        int total = 0;
        int palindromes = 0;
        int word_start = 0;

        for(int i = 0; i <= cnt; i++){
            if(i == cnt || buf[i] == ' '){
                int word_len = i - word_start;
                if (word_len > 0){
                    total++;
                    if(is_palindrom(buf + word_start, word_len)){
                        palindromes++;
                    }
                }
                word_start = i + 1;
            }
        }

        char response[RESPONSE_SIZE];
        int response_len = snprintf(response, sizeof(response), "%d/%d", palindromes, total);
        sendto(sock, response, response_len, 0, (struct sockaddr *) &clnt_addr, clnt_addr_len);

    }

    rc = close(sock);
    if (rc == -1) {
        perror("close");
        return 1;
    }

    return 0;
}
