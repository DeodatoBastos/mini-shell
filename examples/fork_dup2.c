#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// int main (int argc, char *argv[], char *envp[]) {
int main() {
    // pid_t pid = fork();

    // if (pid == 0) {
    //     char pathname[] = "/usr/bin/ls";
    //     char *argv[] = {pathname, "-l", NULL};
    //     char *envp[] = {NULL};

    //     execve(pathname, argv, envp);
    // } else {
    //     wait(NULL);
    // }

    // char buffer[1024] = {0};
    //
    // ssize_t count = read(0, buffer, 1023);

    // if (count == -1) {
    //     char err_buffer[] = "An error occurred in the read.\n";
    //     write(2, err_buffer, sizeof(err_buffer));
    // } else {
    //     write(1, buffer, count);
    // }
    //
    // int fd = open("in.txt", O_RDONLY);

    // printf("fd: %d\n", fd);

    // int fd2 = dup(fd);

    // printf("fd2: %d\n", fd2);

    char buffer[1024] = {0};

    int fd = open("in.txt", O_RDONLY);

    dup2(fd, 0); // change stdin to input file

    scanf("%[^\n]", buffer);

    printf("Buffer: %s\n", buffer);

    int fd_out = open("out.txt", O_WRONLY | O_CREAT, 0644);
    
    dup2(fd_out, 1); // change stdout to output file

    printf("Hello, world!\n");


    return 0;
}
