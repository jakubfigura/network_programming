/* Napisz program w C deklarujący w funkcji main tablicę int liczby[50] i wczytujący do niej z klawiatury kolejne liczby. 
Wczytywanie należy przerwać gdy użytkownik wpisze zero albo gdy skończy się miejsce w tablicy (tzn. po wczytaniu 50 liczb)
Z main należy następnie wywoływać pomocniczą funkcję drukuj, przekazując jej jako argumenty adres tablicy oraz
liczbę wczytanych do niej liczb. Funkcję tę zadeklaruj jako void drukuj(int tablica[], int liczba_elementow).
W jej ciele ma być pętla for drukująca te elementy tablicy, które są większe od 10 i mniejsze od 100..*/
#include <stdio.h>



void drukuj(int tablica[], int liczba_elementow){
    for (int i = 0; i < liczba_elementow; i++){
        if (tablica[i] > 10 && tablica[i] < 100){
            printf("%d ", tablica[i]);
        }
    }
}



int main(void)
{
    int liczby[50];
    int wejście = 1;

    int i = 0;

    while (i < 50 && wejście != 0){
        scanf("%d", &wejście);
        liczby[i] = wejście;
        i++;
    }

    drukuj(liczby, 50);
    
}
