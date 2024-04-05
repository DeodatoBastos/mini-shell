#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// int main(int argc, char *argv[]) {
//     int pipefd[2];
//     pipe(pipefd);

//     if (!fork()) {
//         dup2(pipefd[1], 1);

//         execl("/bin/ls", "ls", "-l", NULL);
//     } else {
//         dup2(pipefd[0], 0);

//         execl("/usr/bin/wc", "wc", "-l", NULL);
//     }

//     printf("This line should not execute!\n");

//     return 0;
// }

int main(int argc, char *argv[]) {
  int pipefd1[2];
  pipe(pipefd1);

  if (!fork()) {
    close(pipefd1[0]);

    dup2(pipefd1[1], 1);

    execl("/bin/ls", "ls", "-l", NULL);
  }

  int pipefd2[2];
  pipe(pipefd2);

  if (!fork()) {
    close(pipefd1[1]);

    dup2(pipefd1[0], 0);
    dup2(pipefd2[1], 1);

    execl("/usr/bin/rev", "rev", NULL);
  }

  close(pipefd1[0]);
  close(pipefd1[1]);

  if (!fork()) {
    close(pipefd2[1]);

    dup2(pipefd2[0], 0);

    execl("/usr/bin/wc", "wc", "-l", NULL);
  }

  close(pipefd2[0]);
  close(pipefd2[1]);

  wait(NULL);

  printf("This line should execute!\n");

  return 0;
}

// int main(int argc, char *argv[]) {
//     int pipefd[2];
//     pipe(pipefd);

//     if (!fork()) {
//         close(pipefd[0]);

//         dup2(pipefd[1], 1);

//         execl("/bin/ls", "ls", "-l", NULL);
//     } else {
//         close(pipefd[1]);

//         dup2(pipefd[0], 0);

//         execl("/usr/bin/wc", "wc", "-l", NULL);
//     }

//     printf("This line should not execute!\n");

//     return 0;
// }
