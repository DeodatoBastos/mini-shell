#include "utils.h"

bool is_in_cmd(const char *str) { return !strcmp(str, IN_CMD); }

bool is_out_cmd(const char *str) { return !strcmp(str, OUT_CMD); }

bool is_pipe_cmd(const char *str) { return !strcmp(str, PIPE_CMD); }

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
