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

int main() {
  int copy_in = STDIN_FILENO, copy_out = STDOUT_FILENO;
  char *envp[] = {"PATH=/bin:/usr/bin:./", NULL};
  char path[] = "/bin/:/usr/bin/:./";
  printf("Welcome to the miniature-shell.\n");

  while (true) {
    printf("\ncmd> ");
    char buffer[1024] = {0};
    if(scanf("%[^\n]", buffer) == EOF) break;

    char *token = strtok(buffer, " ");

    // pid_t pid = fork();

    // if (pid == ERROR_CODE) {
    //   fprintf(stderr, "Error while forking process\n");
    //   continue;
    // }

    // if (pid > 0) {
    //   wait(NULL);
    // }

    // if (pid == 0 && token != NULL) {
    if (!fork() && token != NULL) {
      // get argv
      int fd_in = NO_CHANGES_CODE, fd_out = NO_CHANGES_CODE;
      char *argv[1024];
      argv[0] = strdup(token);
      char *cmd = strdup(token);

      for (int i = 1; token != NULL; i++) {
        token = strtok(NULL, " ");

        // get new stdin
        if (token != NULL && !strcmp(token, "<")) {
          token = strtok(NULL, " ");
          fd_in = open(token, O_RDONLY);
          if (fd_in == ERROR_CODE) {
            fprintf(stderr, "Error while open file: %s.\n", token);
            continue;
          }

          token = strtok(NULL, " ");
        }

        // get new stdout
        if (token != NULL && !strcmp(token, ">")) {
          token = strtok(NULL, " ");
          fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
          if (fd_out == ERROR_CODE) {
            fprintf(stderr, "Error while open file: %s\n", token);
            continue;
          }

          token = strtok(NULL, " ");
        }

        if (token == NULL)
          argv[i] = NULL;
        else
          argv[i] = strdup(token);
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

      // execute the program
      char *pre;
      for (pre = strtok(path, ":"); pre != NULL; pre = strtok(NULL, ":")) {
        char fullpath[strlen(pre) + strlen(cmd) + 1];
        strcpy(fullpath, pre);
        argv[0] = strdup(strcat(fullpath, cmd));

        execve(fullpath, argv, envp);
      }

      if (pre == NULL) {
        argv[0] = cmd;
        if (execve(cmd, argv, envp) == ERROR_CODE) {
          fprintf(stderr, "msh: command not found: %s\n", cmd);
        }
      }

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
