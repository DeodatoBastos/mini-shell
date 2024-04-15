#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

bool is_in_cmd(const char *str) {
    if (str == NULL)
        return false;

    return !strcmp(str, IN_CMD);
}

bool is_out_cmd(const char *str) {
    if (str == NULL)
        return false;
    return !strcmp(str, OUT_CMD);
}

bool is_pipe_cmd(const char *str) {
    if (str == NULL)
        return false;
    return !strcmp(str, PIPE_CMD);
}

void execute(char **argv, char *cmd, char *path, char *const *envp) {
    if (argv == NULL || cmd == NULL || envp == NULL)
        exit(EXIT_FAILURE);

    if (path == NULL) {
        if (execve(cmd, argv, envp) == ERROR_CODE) {
            fprintf(stderr, "msh: command not found: %s\n", cmd);
        }
        return;
    }

    char *pre;
    char *local_path = strdup(path);
    for (pre = strtok(local_path, PATH_DELIMITER); pre != NULL; pre = strtok(NULL, PATH_DELIMITER)) {
        char fullpath[strlen(pre) + strlen(cmd) + 2];
        strcpy(fullpath, pre);
        strcat(fullpath, "/");
        free(argv[0]);
        argv[0] = strdup(strcat(fullpath, cmd));

        execve(fullpath, argv, envp);
    }

    if (pre == NULL) {
        free(argv[0]);
        argv[0] = strdup(cmd);
        if (execve(cmd, argv, envp) == ERROR_CODE) {
            fprintf(stderr, "msh: command not found: %s\n", cmd);
        }
    }
    free(local_path);
}

void execute_cio(char **argv, char *cmd, char *path, char *const *envp, int fd_in, int fd_out) {
    if (!fork()) {
        // Child process
        if (fd_in != STDIN_FILENO) {
            dup2(fd_in, STDIN_FILENO); // Redirect input
            close(fd_in);
        }
        if (fd_out != STDOUT_FILENO) {
            dup2(fd_out, STDOUT_FILENO); // Redirect output
            close(fd_out);
        }
        execute(argv, cmd, path, envp);
        perror("execute");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
}

void file_logging(char *file_name, const char severity, const char *info) {
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

    FILE *file = fopen(file_name, "a");
    fprintf(file, "%s: %d-%02d-%02d %02d:%02d:%02d -- ", msg, tm.tm_year + 1900, tm.tm_mon + 1,
            tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(file, "%s\n", info);
    fclose(file);

    free(msg);
}

void wait_all() {
    while (wait(NULL) > 0);
}

char **split(char *str, char *ch, int *count) {
    char *str_cpy = strdup(str);
    char *token = strtok(str_cpy, ch);

    if (token == NULL) {
        *count = 0;
        return NULL;
    }

    char **commands = malloc(sizeof(char *) * MAX_ARGS);

    // get commands
    int i;
    for (i = 0; token != NULL; i++) {
        commands[i] = strdup(token);
        token = strtok(NULL, ch);
    }
    *count = i;

    free(str_cpy);
    return commands;
}

void trim(char *str) {
    if (str == NULL)
        return;

    while (isspace((unsigned char)str[0]))
        memmove(str, str + 1, strlen(str));

    while (isspace((unsigned char)str[strlen(str) - 1]))
        str[strlen(str) - 1] = '\0';
}
