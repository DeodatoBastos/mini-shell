#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

char *new_string(char *str) {
    char *ret;

    ret = malloc(sizeof(char) * 255);
    strcpy(ret, str);

    return (ret);
}

// TODO: handle erros 

int main() {
    while (true) {
        printf("cmd> ");
        char buffer[1024] = {0};
        scanf("%[^\n]", buffer);
        char* token = strtok(buffer, " ");

        pid_t pid = fork();

        if (pid == 0) {
            int fd_in = -1, fd_out = -1, copy_in = 0, copy_out = 1;
            char *argv[1024];
            argv[0] = new_string(token);

            for (int i = 1; token != NULL; i++) {
                token = strtok(NULL, " ");
                if (token == NULL) {
                    break;
                }

                if (!strcmp(token, "<")) {
                    token = strtok(NULL, " ");
                    fd_in = open(token, O_RDONLY);

                    token = strtok(NULL, " ");
                }

                if (token == NULL) {
                    break;
                }

                if (!strcmp(token, ">")) {
                    token = strtok(NULL, " ");
                    fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                    token = strtok(NULL, " ");
                }

                if (token == NULL) {
                    break;
                }

                argv[i] = new_string(token);
            }
            char *envp[] = {NULL};

            if (fd_in != -1) {
                copy_in = dup(STDIN_FILENO);
                dup2(fd_in, STDIN_FILENO); // change stdin to input fill
                close(fd_in);
            }

            if (fd_out != -1) {
                copy_out = dup(STDOUT_FILENO);
                dup2(fd_out, STDOUT_FILENO); // change stdout to output file
                close(fd_out);
            }

            execve(argv[0], argv, envp);

            if (fd_in != -1) {
                dup2(copy_in, STDIN_FILENO);
                close(copy_in);
            }

            if (fd_out != -1) {
                dup2(copy_out, STDOUT_FILENO);
                close(copy_out);
            }
        } else {
            wait(NULL);
        }
        getchar();
    }

    return 0;
}
