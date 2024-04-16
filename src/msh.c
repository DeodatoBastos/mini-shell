#include "utils.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define NO_CHANGES_CODE -2
#define MAX_LINE_LENGTH 1024
#define MAX_ARGS 64

int main() {
    char *path = get_path();
    char *user = getenv("USER");
    char **envp = get_envp(path);

    printf("Welcome to the miniature-shell.\n");
    char buffer[MAX_LINE_LENGTH] = {0};

    char *log_path = "logs/log.txt";

    while (true) {
        printf("\n[%s] > ", user);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        // Remove trailing newline character if present
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strcmp(buffer, "") == 0)
            continue;

        int copy_in = dup(STDIN_FILENO);
        int copy_out = dup(STDOUT_FILENO);

        int num_cmds;
        char **commands = split(buffer, PIPE_CMD, &num_cmds);
        int fd_in = NO_CHANGES_CODE, fd_out = NO_CHANGES_CODE;
        int input = STDIN_FILENO;
        int pipefd[2];

        for (int i = 0; i < num_cmds; i++) {
            trim(commands[i]);
            char *token = strtok(commands[i], DELIMITER);

            char **argv = malloc(sizeof(char) * MAX_ARGS);
            argv[0] = strdup(token);
            char *cmd = strdup(token);
            int argc = 1;

            // get argv
            for (int j = 1; token != NULL; j++) {
                token = strtok(NULL, DELIMITER);

                // get new stdin
                if (i == 0 && token != NULL && is_in_cmd(token)) {
                    token = strtok(NULL, DELIMITER);
                    fd_in = open(token, O_RDONLY);
                    if (fd_in == ERROR_CODE) {
                        fprintf(stderr, "Error while open file: %s.\n", token);
                        continue;
                    }
                    token = strtok(NULL, DELIMITER);
                }

                // get new stdout
                if (i == num_cmds - 1 && token != NULL && is_out_cmd(token)) {
                    token = strtok(NULL, DELIMITER);
                    fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd_out == ERROR_CODE) {
                        fprintf(stderr, "Error while open file: %s\n", token);
                        continue;
                    }
                    token = strtok(NULL, DELIMITER);
                }

                if (token == NULL)
                    argv[j] = NULL;
                else {
                    argc++;
                    argv[j] = strdup(token);
                }
            }
            // change stdin
            if (i == 0 && fd_in != NO_CHANGES_CODE) {
                copy_in = dup(STDIN_FILENO);
                if (copy_in == ERROR_CODE) {
                    fprintf(stderr, "Error while copying default stdin\n");
                }

                // change stdin to input file
                if (dup2(fd_in, STDIN_FILENO) == ERROR_CODE) {
                    fprintf(stderr, "Error while changing stdin\n");
                }

                if (close(fd_in) == ERROR_CODE) {
                    fprintf(stderr, "Error while closing stdin file\n");
                }
            }

            // change stdout
            if (i == num_cmds - 1 && fd_out != NO_CHANGES_CODE) {
                copy_out = dup(STDOUT_FILENO);
                if (copy_out == ERROR_CODE) {
                    fprintf(stderr, "Error while copying default stdout\n");
                }

                // change stdout to output file
                if (dup2(fd_out, STDOUT_FILENO) == ERROR_CODE) {
                    fprintf(stderr, "Error while changing stdout\n");
                }

                if (close(fd_out) == ERROR_CODE) {
                    fprintf(stderr, "Error while closing stdout file\n");
                }
            }

            char info[strlen("executing: ") + strlen(cmd) + 1];
            sprintf(info, "executing: %s", cmd);
            file_logging(log_path, 'i', info);

            if (i < num_cmds - 1) {
                pipe(pipefd);
                execute_cio(argv, cmd, path, envp, input, pipefd[1]);
                close(pipefd[1]);
                input = pipefd[0];
            } else {
                execute_cio(argv, cmd, path, envp, input, STDOUT_FILENO);
            }
            free(cmd);
            free_char_array(argv);
            file_logging(log_path, 'i', "end of fork");
        }

        close(pipefd[0]);
        close(pipefd[1]);
        file_logging(log_path, 'i', "after loop");

        // comeback to default stdin
        if (dup2(copy_in, STDIN_FILENO) == ERROR_CODE) {
            fprintf(stderr, "Error while changing stdin\n");
        }

        if (close(copy_in) == ERROR_CODE) {
            fprintf(stderr, "Error while closing copy of default stdin\n");
        }

        // comeback to default stdout
        if (dup2(copy_out, STDOUT_FILENO) == ERROR_CODE) {
            fprintf(stderr, "Error while changing stdout\n");
        }

        if (close(copy_out) == ERROR_CODE) {
            fprintf(stderr, "Error while closing copy of default stdout\n");
        }

        free_char_array(commands);
        file_logging(log_path, 'i', "end of while");
    }

    free_char_array(envp);
    file_logging(log_path, 'i', "end of file");
    return 0;
}
