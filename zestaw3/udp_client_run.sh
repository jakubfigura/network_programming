#!/bin/bash

for i in {1..12}
do
    nc -u -w 1 127.0.0.1 2020 < "testy/t$i.txt" > "testy/o$i.txt"
done