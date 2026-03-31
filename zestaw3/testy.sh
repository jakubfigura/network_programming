#!/bin/bash
cd testy
for i in {1..12};
do
plik="t$i.txt"
od -A d -t u1 -t c < "$plik"
echo "\n"
done