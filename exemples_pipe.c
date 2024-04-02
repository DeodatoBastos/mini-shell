#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int pipefd[2];
    pipe(pipefd);

    if (!fork()) {
        dup2(pipefd[1], 1);

        execl("/bin/ls", "ls", "-l", NULL);
    } else {
        dup2(pipefd[0], 0);

        execl("/usr/bin/wc", "wc", "-l", NULL);
    }

    printf("This line should not execute!\n");

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
