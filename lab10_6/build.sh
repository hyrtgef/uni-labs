mkdir build bin lib
echo "s s s s s s s s s s s" >> input.txt 
gcc -Wall -Wextra -Iinclude -O2 -c src/utils.c -o build/utils.o

gcc -Wall -Wextra -Iinclude -O2 -fPIC -shared -o lib/libprocess.so src/process.c

gcc -Wall -Wextra -Iinclude -O2 -c src/server.c -o build/server.o
gcc -Wall -Wextra -Iinclude -O2 -c src/client.c -o build/client.o

gcc build/client.o build/utils.o -lrt -o bin/client
gcc build/server.o build/utils.o -ldl -lrt -o bin/server