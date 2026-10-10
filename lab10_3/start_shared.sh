#!/bin/bash

rm -f src/*.o app_shared parent libprocess.so output*.txt

# libprocess.so
gcc -Wall -Wextra -Iinclude -O2 -fPIC -c src/process.c -o src/process.o
gcc -shared -o libprocess.so src/process.o

# app_shared
gcc -Wall -Wextra -Iinclude -O2 -c src/main_shared.c -o src/main_shared.o
gcc -Wall -Wextra -Iinclude -O2 -c src/utils.c       -o src/utils.o
gcc src/main_shared.o src/utils.o -ldl -o app_shared

# parent
gcc -Wall -Wextra -O2 -c src/parent.c -o src/parent.o
gcc src/parent.o -o parent

echo "Готово"