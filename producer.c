#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int M = 10000;

const int MAX = 26;
const char alphabets[MAX] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
                             'h', 'i', 'j', 'k', 'l', 'm', 'n',
                             'o', 'p', 'q', 'r', 's', 't', 'u',
                             'v', 'w', 'x', 'y', 'z'};

int main(int argc, char* argv[]) {
    M = atoi(argv[1]);
    srand(time(NULL));
    int bytesWrite = 0;

    while (bytesWrite < M) {
        char writing = alphabets[rand() % MAX];
        bytesWrite += write(bytesWrite, &writing, 1);
    }
    return 0;
}
