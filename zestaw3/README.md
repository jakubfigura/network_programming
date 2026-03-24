# Serwer UDP
Program zawiera implementację serwera UDP, który udziela odpowiedzi, czy wysłany datagram zawiera palindromy. Serwer wysyła komunikat w postaci ułamka P/T, gdzie P - oznacza liczbę przesłanych palindromów, natomiast T - całkowitą liczbę przesłanych słów. Jako palindrom traktowany jest wyraz jednoliterowy np. „i" oraz wyrazy pisane z wielkiej litery np. „Ala".

## Kompilacja
Serwer został napisany w języku C. Kompilacja za pomocą:
```
gcc -std=c99 -pedantic -Wall udp_srv_palindrom.c  -o palindrom   
```

## Testowanie
```
netcat -u 127.0.0.1 2020 
```