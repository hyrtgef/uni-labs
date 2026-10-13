gcc -Wall -Wextra -Iinclude -O2 -fPIC -c src/process.c -o src/process_pic.o
gcc -shared -o libprocess.so src/process_pic.o
gcc -Wall -Wextra -Iinclude -O2 -c src/utils.c -o src/utils.o
gcc -Wall -Wextra -Iinclude -O2 -c src/server.c -o src/server.o
gcc src/server.o src/utils.o -ldl -o server
gcc -Wall -Wextra -Iinclude -O2 -c src/client.c -o src/client.o
gcc src/client.o src/utils.o -o client