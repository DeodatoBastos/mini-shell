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

char *new_string(char *str) {
  if (str == NULL)
    return NULL;
  char *ret;

  ret = malloc(sizeof(char) * 255);
  strcpy(ret, str);

  return (ret);
}

int main() {
  int copy_in = STDIN_FILENO, copy_out = STDOUT_FILENO;

  while (true) {
    printf("cmd> ");
    char buffer[1024] = {0};
    scanf("%[^\n]", buffer);
    char *token = strtok(buffer, " ");

    pid_t pid = fork();

    if (pid == ERROR_CODE) {
      fprintf(stderr, "Error while forking process\n");
      continue;
    }

    if (pid > 0) {
      wait(NULL);
      continue;
    }

    if (pid == 0) {

      // get argv
      int fd_in = NO_CHANGES_CODE, fd_out = NO_CHANGES_CODE;
      char *argv[1024];
      argv[0] = new_string(token);

      for (int i = 1; token != NULL; i++) {
        token = strtok(NULL, " ");

        // get new stdin
        if (token != NULL && !strcmp(token, "<")) {
          token = strtok(NULL, " ");
          fd_in = open(token, O_RDONLY);
          if (fd_in == ERROR_CODE) {
            fprintf(stderr, "Error while reading file\n. Make sure that this "
                            "file exists.\n");
            continue;
          }

          token = strtok(NULL, " ");
        }

        // get new stdout
        if (token != NULL && !strcmp(token, ">")) {
          token = strtok(NULL, " ");
          fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
          if (fd_out == ERROR_CODE) {
            fprintf(stderr,
                    "Error while creating the file or writing into file\n");
            continue;
          }

          token = strtok(NULL, " ");
        }

        argv[i] = new_string(token);
      }
      char *envp[] = {NULL};

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

      if (execve(argv[0], argv, envp) == ERROR_CODE) {
        fprintf(stderr, "Error while executing command\n");
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

    getchar();
  }

  return 0;
}
