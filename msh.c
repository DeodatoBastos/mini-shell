#include <fcntl.h>
#include <locale.h>
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

int main() {
  char env_name[5] = "PATH";
  char *path = getenv(env_name);
  if (!path) {
    perror("PATH not found\n");
    exit(EXIT_FAILURE);
  }

  char *envp[] = {NULL};
  char welcome_message[] = "Welcome to the miniature-shell.\n";
  char cmd_message[] = "\ncmd> ";
  write(STDOUT_FILENO, welcome_message, sizeof(welcome_message));

  while (true) {
    write(STDOUT_FILENO, cmd_message, sizeof(cmd_message));
    char buffer[1024];
    ssize_t count = read(STDIN_FILENO, buffer, 1023);

    if (count == -1) {
      char err_buffer[] = "An error occurred in the read.\n";
      write(STDERR_FILENO, err_buffer, sizeof(err_buffer));
    }

    char *token = strtok(buffer, DELIMITER);
    pid_t pid = fork();

    if (pid == ERROR_CODE) {
      perror("Fork\n");
      exit(EXIT_FAILURE);
    }

    if (pid > 0) {
      wait(NULL);
    }

    if (pid == 0) {
      int copy_in = STDIN_FILENO, copy_out = STDOUT_FILENO;
      int fd_in = NO_CHANGES_CODE, fd_out = NO_CHANGES_CODE;

      char *argv[1024];
      int argc = 1;
      argv[0] = strdup(token);
      char *cmd = strdup(token);

      // get argv
      for (int i = 1; token != NULL; i++) {
        token = strtok(NULL, DELIMITER);

        // get new stdin
        if (token != NULL && !strcmp(token, IN_CMD)) {
          token = strtok(NULL, DELIMITER);
          fd_in = open(token, O_RDONLY);
          if (fd_in == ERROR_CODE) {
            perror("Open in file\n");
            exit(EXIT_FAILURE);
          }

          token = strtok(NULL, DELIMITER);
        }

        // get new stdout
        if (token != NULL && !strcmp(token, OUT_CMD)) {
          token = strtok(NULL, DELIMITER);
          fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
          if (fd_out == ERROR_CODE) {
            perror("Open out file\n");
            exit(EXIT_FAILURE);
          }

          token = strtok(NULL, DELIMITER);
        }

        if (token != NULL) {
          argv[i] = strdup(token);
          argc++;
        }
      }

      // change stdin
      if (fd_in != NO_CHANGES_CODE) {
        copy_in = dup(STDIN_FILENO);
        if (copy_in == ERROR_CODE) {
          perror("dup stdin\n");
          exit(EXIT_FAILURE);
        }

        // change stdin to input file
        if (dup2(fd_in, STDIN_FILENO) == ERROR_CODE) {
          perror("dup2 stdin\n");
          exit(EXIT_FAILURE);
        }

        if (close(fd_in) == ERROR_CODE) {
          perror("close in file\n");
          exit(EXIT_FAILURE);
        }
      }

      // change stdout
      if (fd_out != NO_CHANGES_CODE) {
        copy_out = dup(STDOUT_FILENO);
        if (copy_out == ERROR_CODE) {
          perror("dup stdout\n");
          exit(EXIT_FAILURE);
        }

        // change stdout to output file
        if (dup2(fd_out, STDOUT_FILENO) == ERROR_CODE) {
          perror("dup2 stdout\n");
          exit(EXIT_FAILURE);
        }

        if (close(fd_out) == ERROR_CODE) {
          perror("close out file\n");
          exit(EXIT_FAILURE);
        }
      }

      // execute the program
      if (strchr(cmd, '/')) {
        execve(cmd, argv, envp);
        fprintf(stderr, "msh: command not found: %s without path\n", cmd);
        exit(EXIT_FAILURE);
      } else if (path != NULL) {
        char *pre;
        char *local_path = strdup(path);

        for (pre = strtok(local_path, PATH_DELIMITER); pre != NULL;
             pre = strtok(NULL, PATH_DELIMITER)) {
          int size = strlen(pre) + strlen(cmd) + 2;
          char fullpath[size];
          strcpy(fullpath, pre);
          strcat(fullpath, "/");
          argv[0] = realloc(argv[0], sizeof(char *) * (size));
          strcpy(argv[0], strcat(fullpath, cmd));

          if (execve(fullpath, argv, envp) != ERROR_CODE) {
            break;
          }
        }
        free(local_path);

        if (pre == NULL) {
          fprintf(stderr, "msh: command not found: %s\n", cmd);
          exit(EXIT_FAILURE);
        }
      }

      // comeback to default stdin
      if (fd_in != NO_CHANGES_CODE) {
        if (dup2(copy_in, STDIN_FILENO) == ERROR_CODE) {
          perror("dup2 stdin back\n");
          exit(EXIT_FAILURE);
        }

        if (close(copy_in) == ERROR_CODE) {
          perror("close copy stdin\n");
          exit(EXIT_FAILURE);
        }
      }

      // comeback to default stdout
      if (fd_out != NO_CHANGES_CODE) {
        if (dup2(copy_out, STDOUT_FILENO) == ERROR_CODE) {
          perror("dup2 stdout back\n");
          exit(EXIT_FAILURE);
        }

        if (close(copy_out) == ERROR_CODE) {
          perror("close copy stdout\n");
          exit(EXIT_FAILURE);
        }
      }

      free(cmd);
      for (int i = 0; i < argc; i++)
        free(argv[i]);
    }

    // getchar();
  }

  return 0;
}
