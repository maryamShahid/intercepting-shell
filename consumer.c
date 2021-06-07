#include <unistd.h>
#include <stdlib.h>

int M = 10000;

int main(int argc, char* argv[]) {
    M = atoi(argv[1]);
    int bytesRead = 0;

    while (bytesRead < M) {
        char reading;
        bytesRead += read(STDIN_FILENO, &reading , 1);
    }
    return 0;
}