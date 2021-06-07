#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define STR_LEN 1000
unsigned int N = 1000;
int number, position;

char* readLine() {
    char* inputLine = malloc(sizeof(char[STR_LEN]));
    fgets(inputLine, STR_LEN, stdin);

    if (inputLine == NULL) {
        perror("Unable to read command\n");
        exit(1);
    }
    return inputLine;
}

char** parseCommand(char *line) {
    number = 0;
    int index = 0;
    char **argv = malloc(number * sizeof(char *));
    char *token;
    char *buffer = strdup(line);

    if (argv == NULL) {
        fprintf(stderr, "memory allocation error\n");
        exit(1);
    }

    token = strtok(buffer, " \n");
    number++;

    while (token != NULL) {
        argv = realloc(argv, sizeof(char *) * number);
        if (argv == NULL) {
            fprintf(stderr, "memory allocation error\n");
            exit(1);
        }
        argv[index++] = token;
        token = strtok(NULL, " \n");
        number++;
    }
    argv = realloc(argv, sizeof(char *) * number);
    argv[index] = NULL;

    return argv;
}

int runCommand(char **args) {
    pid_t n;
    n = fork();

    if (n < 0) {
        // fork failed
        fprintf(stderr, "Fork Failed\n");
        exit(1);
    } else if (n == 0) {
        // child process
        execvp(args[0], args);
        fprintf(stderr, "Cannot run command\n");
    } else {
        // parent process
        wait(NULL);
    }
    return 1;
}

int checkCommand(char* line) {
    int found = 0;
    position = 0;

    char **tokens = parseCommand(line);

    while (tokens[position] != NULL) {
        if (strchr(tokens[position], '|')) {
            found = 1;
            break;
        }
        position++;
    }
    return found;
}

int runNormalMode(char *line) {
    char **tokens = parseCommand(line);
    int check, first, second;
    second = 0;
    first = position + 1;
    check = number - position;

    char **str1 = malloc((first) * sizeof(char *));
    char **str2 = malloc((check) * sizeof(char *));

    for (int x = 0; x < position; x++) {
        str1[x] = tokens[x];
    }
    for (int y = (first); y < number; y++) {
        str2[second] = tokens[y];
        second++;
    }

    str1[position] = NULL;
    str2[check] = NULL;

    // measure time of experiment
    srand(time(0));
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int pipe1[2];
    if (pipe(pipe1) == -1) {
        fprintf(stderr, "\nError");
        exit(1);
    }
    pid_t n1;
    n1 = fork();
    if (n1 < 0) {
        // fork failed
        fprintf(stderr, "\nFork Failed");
        exit(1);
    }
    if (n1 == 0) {
        // child process 1
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        if (execvp(str1[0], str1) == -1) {
            fprintf(stderr, "Cannot run command\n");
            exit(2);
        }
    }

    pid_t n2;
    n2 = fork();
    if (n2 < 0) {
        // fork failed
        fprintf(stderr, "\nFork Failed");
        exit(3);
    }
    if (n2 == 0) {
        // child process 2
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);
        if (execvp(str2[0], str2) == -1) {
            fprintf(stderr, "Cannot run command\n");
        }
    }

    close(pipe1[0]);
    close(pipe1[1]);

    waitpid(n1, NULL, 0);
    waitpid(n2, NULL, 0);

    // get time of experiment
    gettimeofday(&end, NULL);
    int seconds = (end.tv_sec - start.tv_sec);
    int micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
    printf("Time taken: %d s and %d ms\n", seconds, micros);

    return 0;
}

void runTappedMode(char *line) {
    char **tokens = parseCommand(line);
    int check, first, second;
    second = 0;
    first = position + 1;
    check = number - position;

    char **str1 = malloc((first) * sizeof(char *));
    char **str2 = malloc((check) * sizeof(char *));

    for (int x = 0; x < position; x++) {
        str1[x] = tokens[x];
    }
    for (int y = (first); y < number; y++) {
        str2[second] = tokens[y];
        second++;
    }

    str1[position] = NULL;
    str2[check] = NULL;

    // measure time of experiment
    srand(time(0));
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1) {
        fprintf(stderr, "Error\n");
        exit(1);
    }

    if (pipe(pipe2) == -1) {
        fprintf(stderr, "Error\n");
        exit(1);
    }

    // child process 1
    pid_t n1;
    n1 = fork();
    if (n1 < 0) {
        // fork failed
        fprintf(stderr, "Fork Failed\n");
        exit(1);
    } else if (n1 == 0) {
        close(pipe2[0]); // close unused ends
        close(pipe2[1]);
        close(pipe1[0]);
        dup2(pipe1[1], STDOUT_FILENO);
        if (execvp(str1[0], str1) == -1) {
            fprintf(stderr, "Cannot run first command\n");
            exit(1);
        }
    } else {
        // child process 2
        pid_t n2;
        n2 = fork();

        if (n2 < 0) {
            // fork failed
            fprintf(stderr, "Fork Failed\n");
            exit(3);
        } else if (n2 == 0) {
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[1]);
            dup2(pipe2[0], STDIN_FILENO);
            if (execvp(str2[0], str2) == -1) {
                fprintf(stderr, "Cannot run second command\n");
                exit(1);
            }
        } else {
            // parent process
            int characterCount = 0;
            int bytesRead;
            int count = 0;

            close(pipe1[1]);
            close(pipe2[0]);

            char *buffer;
            buffer = malloc(sizeof(char) * N);

            while ((bytesRead = read(pipe1[0], &buffer, N)) > 0) {
                write(pipe2[1], &buffer, bytesRead);
                characterCount = characterCount + bytesRead;
                count++;
            }

            close(pipe1[0]);
            close(pipe2[1]);

            waitpid(n1, NULL, 0);
            waitpid(n2, NULL, 0);

            // get time of experiment
            gettimeofday(&end, NULL);
            int seconds = (end.tv_sec - start.tv_sec);
            int micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
            printf("Time taken: %d s and %d ms\n", seconds, micros);

            printf("\ncharacter-count: %d\nread-call-count: %d\nwrite-call-count: %d\n",
                   characterCount, count, count);
        }
    }
}

void userLoop(int mode) {
    while (1) {
        printf("isp:~$ ");
        char *line = readLine();
        char **args;

        if (checkCommand(line) == 1) {
            if (mode == 1) {
                runNormalMode(line);
            }
            if (mode == 2) {
                runTappedMode(line);
            }
            continue;
        } else {
            args = parseCommand(line);
        }
        // check if command is empty
        if (args[0] == NULL) {
            continue;
        }
        // check for exit
        if (strcmp(args[0], "exit") == 0) {
            break;
        }
        runCommand(args);
    }
}

int main(int argc, char *argv[]) {
    N = atoi(argv[1]);
    int mode = atoi(argv[2]);

    if (!(mode == 1 | mode == 2)) {
        fprintf(stderr, "Mode can be 1 (normal mode) or 2 (tapped mode)\n");
        exit(1);
    }
    if (N < 1 || N > 4096) {
        printf("N should be between 1 and 4096\n");
        exit(1);
    }
    userLoop(mode);
}




