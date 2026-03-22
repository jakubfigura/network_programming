/*
Opracuj funkcję sprawdzającą, czy przekazany jej bufor zawiera tylko i wyłącznie drukowalne znaki ASCII,
tzn. bajty o wartościach z przedziału domkniętego [32, 126].
Sygnatura: bool is_printable_buf(const void * buf, int len)

Opracuj alternatywną wersję funkcji biorącą łańcuch w sensie języka C (ciąg niezerowych bajtów zakończony zerem).
Sygnatura: bool is_printable_str(const char * str)
*/

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>


bool is_printable_buf(const void *buf, int len){
    /* funckaj sprawdajaca, czy przekazany bufor zawiera tylko drukowalne znaki*/
    /* zwraca false jezeli nie jest drukowalna*/
    /* zwraca true jezeli jest*/
    const unsigned char *p = (const unsigned char *) buf;
    for(int i=0; i < len; i++){
        if(*p < 32 || *p > 126){
            return false;
        }
        p++;
    }
    return true;
}

bool is_printable_str(const char *str){
    while(*str != '\0'){
        if(*str < 32 || *str > 126){
            return false;
        }
        str++;

    }
    return true;
}


int main(void){

    void *buf = "AB12CDEFGH? kfla{c'?#$67";

    printf("%d\n", is_printable_buf(buf, 24));

    printf("%d\n", is_printable_str(buf));
   
    return 0;
}
