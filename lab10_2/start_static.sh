#!/bin/bash

rm -rf output.txt
rm -rf libprocess.so
rm -rf app_static
rm -rf src/utils.o
rm -rf src/process.o
rm -rf src/main_static.o

gcc -Wall -Wextra -Iinclude -O2 -c src/utils.c -o src/utils.o
gcc -Wall -Wextra -Iinclude -O2 -c src/main_static.c -o src/main_static.o
gcc  -Wall -Wextra -Iinclude -O2 -c src/process.c -o src/process.o

ar rcs libprocess.a src/process.o

#работает если рядом не лежит .so
gcc src/main_static.o src/utils.o -L. -lprocess -o app_static

# явный путь к .a
# gcc src/main_static.o src/utils.o ./libprocess.a -o app_static