/*
Przypomnij sobie wiadomości o wskaźnikach i arytmetyce wskaźnikowej w C.
Napisz alternatywną wersję funkcji drukującej liczby, o sygnaturze void drukuj_alt(int * tablica, int liczba_elementow).
Nie używaj w niej indeksowania zmienną całkowitoliczbową (nie może się więc pojawić ani tablica[i], ani *(tablica+i)),
zamiast tego użyj wskaźnika przesuwanego z elementu na element przy pomocy ++.
*/
#include <stdio.h>

void drukuj_alt(int *tablica, int liczba_elementow){
    for(int i = 0; i < liczba_elementow; i++){
        printf("%d\n", *tablica);
        tablica++;
    }
}

int main(void){
    int a[] = {1, 2, 3, 5, 7};
    drukuj_alt(a, 5);

    return 0;
}
