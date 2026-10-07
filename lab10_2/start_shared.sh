#!/bin/bash

rm -f output.txt
rm -f libprocess.a
rm -f app_shared
rm -f src/utils.o
rm -f src/process_pic.o
rm -f src/main_shared.o
rm -f libprocess.so

gcc -Wall -Wextra -Iinclude -O2 -c src/utils.c        -o src/utils.o
gcc -Wall -Wextra -Iinclude -O2 -c src/main_shared.c  -o src/main_shared.o
gcc -Wall -Wextra -Iinclude -O2 -fPIC -c src/process.c -o src/process.o

gcc -shared -o libprocess.so src/process.o

gcc src/main_shared.o src/utils.o -ldl -o app_shared

echo "Сборка app_shared завершена"