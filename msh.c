#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NO_CHANGES_CODE -2
#define ERROR_CODE -1
#define DELIMITER " "
#define PATH_DELIMITER ":"
#define IN_CMD "<"
#define OUT_CMD ">"
#define PIPE_CMD "|"
#define MAX_LINE_LENGTH 1024
#define MAX_ARGS 64

/**
 * @brief verify if a string is a in command: '<'
 *
 * @param str string to verify
 * @return true if is '<' else false
 */
bool is_in_cmd(const char *str) { return !strcmp(str, IN_CMD); }

/**
 * @brief verify if a string is a out command: '>'
 *
 * @param str string to verify
 * @return true if is '>' else false
 */
bool is_out_cmd(const char *str) { return !strcmp(str, OUT_CMD); }

/**
 * @brief verify if a string is a pipe: '>'
 *
 * @param str string to verify
 * @return true if is '|' else false
 */
bool is_pipe_cmd(const char *str) { return !strcmp(str, PIPE_CMD); }

/**
 * @brief execute a command
 *
 * @param argv all the orguments of the command
 * @param cmd the command
 * @param path the path envirioment variable
 * @param envp a list of envirioment variable
 */
void execute(char **argv, char *cmd, char *path, char *const *envp) {
    char *pre;
    for (pre = strtok(path, PATH_DELIMITER); pre != NULL; pre = strtok(NULL, PATH_DELIMITER)) {
        char fullpath[strlen(pre) + strlen(cmd) + 1];
        strcpy(fullpath, pre);
        argv[0] = strdup(strcat(fullpath, cmd));

        execve(fullpath, argv, envp);
    }

    if (pre == NULL) {
        argv[0] = strdup(cmd);
        if (execve(cmd, argv, envp) == ERROR_CODE) {
            fprintf(stderr, "msh: command not found: %s\n", cmd);
        }
    }
}

/**
 * @brief loggin info into a file
 *
 * @param file pointer to file
 * @param severity 'i' for info, 'w' for warning and 'e' for error
 * @param info the information to be loged
 */
void file_logging(FILE *file, const char severity, const char *info) {
    if (info == NULL)
        exit(EXIT_FAILURE);

    char *msg;
    switch (severity) {
    case 'i':
        msg = strdup("INFO");
        break;
    case 'w':
        msg = strdup("WARNING");
        break;
    case 'e':
        msg = strdup("ERROR");
        break;
    default:
        exit(EXIT_FAILURE);
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(file, "%s: %d-%02d-%02d %02d:%02d:%02d -- ", msg, tm.tm_year + 1900, tm.tm_mon + 1,
            tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(file, "executing: %s\n", info);
}

int main() {
    int copy_in, copy_out;

    char *envp[] = {"PATH=/bin:/usr/bin:./", NULL};
    char path[] = "/bin/:/usr/bin/:./";

    printf("Welcome to the miniature-shell.\n");
    char buffer[MAX_LINE_LENGTH] = {0};

    char *log_path = "logs/log.txt";
    FILE *log_file = fopen(log_path, "a");

    while (true) {
        printf("\ncmd> ");
        // fflush(stdout); // Flush the output buffer to ensure prompt is printed

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        // Remove trailing newline character if present
        buffer[strcspn(buffer, "\n")] = '\0';

        // if (scanf("%[^\n]", buffer) == EOF)
        //   break;

        char *token = strtok(buffer, DELIMITER);
        if (token != NULL) {
            int fd_in = NO_CHANGES_CODE, fd_out = NO_CHANGES_CODE;
            char *first_argv[MAX_ARGS];
            int first_argc = 1;
            first_argv[0] = strdup(token);
            char *first_cmd = strdup(token);

            // get first argv
            for (int i = 1; token != NULL && !is_pipe_cmd(token); i++) {
                token = strtok(NULL, DELIMITER);

                // get new stdin
                if (token != NULL && !is_pipe_cmd(token) && is_in_cmd(token)) {
                    token = strtok(NULL, DELIMITER);
                    fd_in = open(token, O_RDONLY);
                    if (fd_in == ERROR_CODE) {
                        fprintf(stderr, "Error while open file: %s.\n", token);
                        continue;
                    }

                    token = strtok(NULL, DELIMITER);
                }

                // get new stdout
                if (token != NULL && !is_pipe_cmd(token) && is_out_cmd(token)) {
                    token = strtok(NULL, DELIMITER);
                    fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd_out == ERROR_CODE) {
                        fprintf(stderr, "Error while open file: %s\n", token);
                        continue;
                    }

                    token = strtok(NULL, DELIMITER);
                }

                if (token == NULL || is_pipe_cmd(token))
                    first_argv[i] = NULL;
                else {
                    first_argc++;
                    first_argv[i] = strdup(token);
                }
            }

            char *second_argv[MAX_ARGS];
            int second_argc = 0;
            char *second_cmd;

            // get second argv
            if (token != NULL && is_pipe_cmd(token)) {
                token = strtok(NULL, DELIMITER);
                second_argv[0] = strdup(token);
                second_cmd = strdup(token);
                second_argc++;

                for (int i = 1; token != NULL && !is_pipe_cmd(token); i++) {
                    token = strtok(NULL, DELIMITER);

                    // get new stdout
                    if (token != NULL && !is_pipe_cmd(token) && is_out_cmd(token)) {
                        token = strtok(NULL, DELIMITER);
                        fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_out == ERROR_CODE) {
                            fprintf(stderr, "Error while open file: %s\n", token);
                            continue;
                        }

                        token = strtok(NULL, DELIMITER);
                    }

                    if (token == NULL || is_pipe_cmd(token))
                        second_argv[i] = NULL;
                    else {
                        second_argc++;
                        second_argv[i] = strdup(token);
                    }
                }
            }

            char *third_argv[MAX_ARGS];
            int third_argc = 0;
            char *third_cmd;

            // get third argv
            if (token != NULL && is_pipe_cmd(token)) {
                token = strtok(NULL, DELIMITER);
                third_argv[0] = strdup(token);
                third_cmd = strdup(token);
                third_argc++;

                for (int i = 1; token != NULL && !is_pipe_cmd(token); i++) {
                    token = strtok(NULL, DELIMITER);

                    // get new stdout
                    if (token != NULL && !is_pipe_cmd(token) && is_out_cmd(token)) {
                        token = strtok(NULL, DELIMITER);
                        fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (fd_out == ERROR_CODE) {
                            fprintf(stderr, "Error while open file: %s\n", token);
                            continue;
                        }

                        token = strtok(NULL, DELIMITER);
                    }

                    if (token == NULL)
                        third_argv[i] = NULL;
                    else {
                        third_argc++;
                        third_argv[i] = strdup(token);
                    }
                }

                third_cmd = strdup(third_argv[0]);
            }

            // change stdin
            if (fd_in != NO_CHANGES_CODE) {
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
            if (fd_out != NO_CHANGES_CODE) {
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

            int pipefd1[2];
            pipe(pipefd1);

            // execute first program
            if (!fork()) {
                close(pipefd1[0]);

                if (second_argc)
                    dup2(pipefd1[1], STDOUT_FILENO);
                else
                    close(pipefd1[1]);

                file_logging(log_file, 'i', first_cmd);
                execute(first_argv, first_cmd, path, envp);
            }

            int pipefd2[2];
            pipe(pipefd2);

            // execute second program
            if (second_argc && !fork()) {
                close(pipefd1[1]);

                dup2(pipefd1[0], STDIN_FILENO);
                if (third_argc)
                    dup2(pipefd2[1], STDOUT_FILENO);
                else
                    close(pipefd2[1]);

                file_logging(log_file, 'i', second_cmd);
                execute(second_argv, second_cmd, path, envp);
            }

            close(pipefd1[0]);
            close(pipefd1[1]);

            // execute third program
            if (third_argc && !fork()) {
                close(pipefd2[1]);

                dup2(pipefd2[0], STDIN_FILENO);

                file_logging(log_file, 'i', third_cmd);
                execute(third_argv, third_cmd, path, envp);
            }

            close(pipefd2[0]);
            close(pipefd2[1]);

            wait(NULL);

            // comeback to default stdin
            if (fd_in != NO_CHANGES_CODE) {
                if (dup2(copy_in, STDIN_FILENO) == ERROR_CODE) {
                    fprintf(stderr, "Error while changing stdin\n");
                }

                if (close(copy_in) == ERROR_CODE) {
                    fprintf(stderr, "Error while closing copy of default stdin\n");
                }
            }

            // comeback to default stdout
            if (fd_out != NO_CHANGES_CODE) {
                if (dup2(copy_out, STDOUT_FILENO) == ERROR_CODE) {
                    fprintf(stderr, "Error while changing stdout\n");
                }

                if (close(copy_out) == ERROR_CODE) {
                    fprintf(stderr, "Error while closing copy of default stdout\n");
                }
            }
        }
    }

    fclose(log_file);

    return 0;
}
