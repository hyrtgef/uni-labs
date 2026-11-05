#!/bin/bash

mkdir -p lib build bin

printf 'a b c d e' > input1.txt
printf 'hello world foo bar' > input2.txt
printf 'one two three' > input3.txt

gcc -Wall -Wextra -Iinclude -O2 -fPIC -shared -o lib/libprocess.so src/process.c

gcc -Wall -Wextra -Iinclude -O2 -c src/utils.c -o build/utils.o

gcc -Wall -Wextra -Iinclude -O2 -c src/main.c   -o build/main.o
gcc -Wall -Wextra -Iinclude -O2 -c src/worker.c -o build/worker.o

gcc build/main.o build/worker.o build/utils.o -lpthread -ldl -o bin/app
