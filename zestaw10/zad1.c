#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
 
#define BUFSIZE 1024
// program kompilujemy poprzez gcc -Wall -ansi -pedantic zad1.c -o zad1
// uruchamiamy program podając jako argument stworzony wcześniej plik tekstowy 
// printf powinien wypisywać liczbę zwracanych bajtow
 
int main (int argc, char **argv) {

    if(argc < 2){
        fprintf(stderr, "Uzycie: zad1.c <argument> ");
    }

    int f1, c;
    char b[BUFSIZE], *n1;
 
    c = 10;
    n1 = argv[1];
 
    f1 = open(n1, O_RDONLY);
    
    if(f1 == -1){
        return 1;
    }

    c = read(f1, b, c);

    if(c == -1){
        perror("read");
        return 0;
    }

    printf("%s: Przeczytano %d znaków z pliku %s: \"%s\"\n",
	   argv[0], c, n1, b);
    close(f1);
 
    return(0);
}


