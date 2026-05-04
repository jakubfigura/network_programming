'''
Napisz program sprawdzający, czy pewna określona witryna działa poprawnie.
Sprawdzenie ma polegać na pobraniu strony spod ustalonego adresu (np. spod http://th.if.uj.edu.pl/).
Proszę nie zapomnieć o zweryfikowaniu, czy na pewno udało się ją poprawnie pobrać (status 200) i czy to jest strona HTML (typ text/html).
Następnie należy sprawdzić, czy rzeczywiście jest to spodziewana strona, a nie np. komunikat o wewnętrznym błędzie serwera WWW
 — to można zweryfikować sprawdzając czy w pobranej treści znajduje się pewien zadany z góry ciąg znaków (np. „Institute of Theoretical Physics”).

Program, w zależności od wyniku sprawdzenia, musi zwracać jako wynik funkcji main kod sukcesu (zero) bądź porażki (wartość większa od zera).
Osoby piszące w Pythonie powinny użyć sys.exit(0) albo sys.exit(1).
Programy tego typu używane są w systemach monitorowania usług sieciowych.
Jeśli na filmie z centrum zarządzania siecią widać ekran z listą serwerów i usług, a przy nich zielone komunikaty „OK” i gdzieniegdzie
czerwone komunikaty błędów, to za tymi kolorami kryją się uruchamiane w regularnych odstępach czasu programy sprawdzające status danej usługi.
'''

import requests
import sys

def main():

    if len(sys.argv) == 3:
        url = sys.argv[1]
        req = requests.get(url)
        expected_content = sys.argv[2]
    else:
        print("Missing required arguments: zadanie2.py <url> <string with expected page content>")
        sys.exit(1)


    if req.status_code != 200:
        print(f"HTTP error: {req.status_code}")
        sys.exit(1)

    if "text/html" not in req.headers.get('Content-Type', ''):
        print("Wrong content type")
        sys.exit(1)

    if expected_content not in req.text:
        print(f"Expected string not found: {expected_content!r}")
        sys.exit(1)

    print(f'Page {url} is working')
    sys.exit(0)


if __name__ == "__main__":
    main()

# przykładowe wywołanie 
# python3 zadanie2.py http://th.if.uj.edu.pl/ "Institute of Theoretical Physics"