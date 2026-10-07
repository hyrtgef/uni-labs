#!/bin/bash

rm -f output.txt
rm -f libprocess.so
rm -f app_static
rm -f src/utils.o
rm -f src/process.o
rm -f src/main_static.o
rm -f libprocess.a

gcc -Wall -Wextra -Iinclude -O2 -c src/utils.c -o src/utils.o
gcc -Wall -Wextra -Iinclude -O2 -c src/main_static.c -o src/main_static.o
gcc -Wall -Wextra -Iinclude -O2 -c src/process.c -o src/process.o

ar rcs libprocess.a src/process.o

#работает если рядом не лежит .so
gcc src/main_static.o src/utils.o -L. -lprocess -o app_static

# явный путь к .a
# gcc src/main_static.o src/utils.o ./libprocess.a -o app_static

echo "Сборка app_static завершена"