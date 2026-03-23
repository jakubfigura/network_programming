/*
Zaimplementuj program kopiujący dane z pliku do pliku przy pomocy funkcji POSIX:
open, read, write, close.
Nazwy plików są podawane jako argumenty programu (argv[1] = źródło, argv[2] = cel).
Każde wywołanie funkcji we-wy jest opatrzone testem błędu; błędy raportowane przez perror.
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(stderr, "podaj plik_zrodlowy i plik_docelowy\n");
        return 1;
    }

    int fd_src = open(argv[1], O_RDONLY);
    if(fd_src == -1){
        perror("open");
        return 1;
    }

    int fd_dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd_dst == -1){
        perror("open");
        return 1;
    }

    /* tutaj mamy bufor z zadanym rozmiarem*/
    char buf[4096];

    ssize_t data_size;

    /*czytamy dane z pliku i zapisujemy*/
    while((data_size = read(fd_src, buf, sizeof(buf))) > 0){
        /* write zwraca liczbe pisanych bajtow*/
        ssize_t written = write(fd_dst, buf, data_size);
        printf("Kopiowanie...\n");
        sleep(1); 
        if(written == -1){
            perror("write");
            return 1;
        }
    }

    if(data_size ==-1){
        perror("read");
        return 1;
    }

    close(fd_src);
    close(fd_dst);







    return 0;

}
