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

// verify if str is as pipe, i.e., "|"
bool is_pipe(const char *str) { return !strcmp(str, PIPE_CMD); }

void execute(char **argv, char *cmd, char *path, char *const *envp) {
  char *pre;
  for (pre = strtok(path, PATH_DELIMITER); pre != NULL;
       pre = strtok(NULL, PATH_DELIMITER)) {
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

int main() {
  int copy_in = STDIN_FILENO, copy_out = STDOUT_FILENO;
  char *envp[] = {"PATH=/bin:/usr/bin:./", NULL};
  char path[] = "/bin/:/usr/bin/:./";
  printf("Welcome to the miniature-shell.\n");

  while (true) {
    printf("\ncmd> ");
    char buffer[1024] = {0};
    if (scanf("%[^\n]", buffer) == EOF)
      break;

    char *token = strtok(buffer, DELIMITER);
    // pid_t pid = fork();

    if (token != NULL) {
      int fd_in = NO_CHANGES_CODE, fd_out = NO_CHANGES_CODE;
      char *first_argv[1024];
      int first_argc = 1;
      first_argv[0] = strdup(token);
      char *first_cmd = strdup(token);

      // get first argv
      for (int i = 1; token != NULL; i++) {
        token = strtok(NULL, DELIMITER);

        // get new stdin
        if (token != NULL && !is_pipe(token) && !strcmp(token, IN_CMD)) {
          token = strtok(NULL, DELIMITER);
          fd_in = open(token, O_RDONLY);
          if (fd_in == ERROR_CODE) {
            fprintf(stderr, "Error while open file: %s.\n", token);
            continue;
          }

          token = strtok(NULL, DELIMITER);
        }

        // get new stdout
        if (token != NULL && !is_pipe(token) && !strcmp(token, OUT_CMD)) {
          token = strtok(NULL, DELIMITER);
          fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
          if (fd_out == ERROR_CODE) {
            fprintf(stderr, "Error while open file: %s\n", token);
            continue;
          }

          token = strtok(NULL, DELIMITER);
        }

        if (token == NULL || !is_pipe(token))
          first_argv[i] = NULL;
        else {
          first_argc++;
          first_argv[i] = strdup(token);
        }
      }

      char *second_argv[1024];
      int second_argc = 0;
      char *second_cmd;

      // get second argv
      if (token != NULL && !!is_pipe(token)) {
        for (int i = 0; token != NULL && !is_pipe(token); i++) {
          token = strtok(NULL, DELIMITER);

          // get new stdout
          if (token != NULL && !is_pipe(token) && !strcmp(token, OUT_CMD)) {
            token = strtok(NULL, DELIMITER);
            fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == ERROR_CODE) {
              fprintf(stderr, "Error while open file: %s\n", token);
              continue;
            }

            token = strtok(NULL, DELIMITER);
          }

          if (token == NULL || !is_pipe(token))
            second_argv[i] = NULL;
          else {
            second_argc++;
            second_argv[i] = strdup(token);
          }
        }

        second_cmd = strdup(second_argv[0]);
      }

      char *third_argv[1024];
      int third_argc = 0;
      char *third_cmd;

      // get thid argv
      if (token != NULL && is_pipe(token)) {
        for (int i = 0; token != NULL && !is_pipe(token); i++) {
          token = strtok(NULL, DELIMITER);

          // get new stdout
          if (token != NULL && !is_pipe(token) && !strcmp(token, OUT_CMD)) {
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

        char *pre;
        for (pre = strtok(path, PATH_DELIMITER); pre != NULL;
             pre = strtok(NULL, ":")) {
          char fullpath[strlen(pre) + strlen(first_cmd) + 1];
          strcpy(fullpath, pre);
          first_argv[0] = strdup(strcat(fullpath, first_cmd));

          execve(fullpath, first_argv, envp);
        }

        if (pre == NULL) {
          first_argv[0] = strdup(first_cmd);
          if (execve(first_cmd, first_argv, envp) == ERROR_CODE) {
            fprintf(stderr, "msh: command not found: %s\n", first_cmd);
          }
        }
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

        char *pre;
        for (pre = strtok(path, PATH_DELIMITER); pre != NULL;
             pre = strtok(NULL, ":")) {
          char fullpath[strlen(pre) + strlen(second_cmd) + 1];
          strcpy(fullpath, pre);
          second_argv[0] = strdup(strcat(fullpath, second_cmd));

          execve(fullpath, second_argv, envp);
        }

        if (pre == NULL) {
          second_argv[0] = strdup(second_cmd);
          if (execve(second_cmd, second_argv, envp) == ERROR_CODE) {
            fprintf(stderr, "msh: command not found: %s\n", second_cmd);
          }
        }
      }

      close(pipefd1[0]);
      close(pipefd1[1]);

      // execute third program
      if (third_argc && !fork()) {
        close(pipefd2[1]);

        dup2(pipefd2[0], STDIN_FILENO);

        char *pre;
        for (pre = strtok(path, PATH_DELIMITER); pre != NULL;
             pre = strtok(NULL, ":")) {
          char fullpath[strlen(pre) + strlen(third_cmd) + 1];
          strcpy(fullpath, pre);
          third_argv[0] = strdup(strcat(fullpath, third_cmd));

          execve(fullpath, third_argv, envp);
        }

        if (pre == NULL) {
          third_argv[0] = strdup(third_cmd);
          if (execve(third_cmd, third_argv, envp) == ERROR_CODE) {
            fprintf(stderr, "msh: command not found: %s\n", third_cmd);
          }
        }
      }

      close(pipefd2[0]);
      close(pipefd2[1]);

      // wait(NULL);

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

    wait(NULL);
    getchar();
  }

  return 0;
}
