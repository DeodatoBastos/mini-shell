#include "../src/utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_file_logging() {
    char *file = "log/test.txt";

    file_logging(file, 'i', "test");
    file_logging(file, 'w', "test");
    file_logging(file, 'e', "test");

    return 0;
}

int test_execute() {
    char *argv[] = {"ls", "-la", NULL};
    char *envp[] = {NULL};
    char path[] = "/bin/:/usr/bin/:./";
    execute(argv, "ls", path, envp);

    return 0;
}

// Test function for is_in_cmd
void test_is_in_cmd() {
    assert(is_in_cmd("<") == true);
    assert(is_in_cmd(">") == false);
    assert(is_in_cmd("|") == false);
    assert(is_in_cmd("command") == false);
}

// Test function for is_out_cmd
void test_is_out_cmd() {
    assert(is_out_cmd(">") == true);
    assert(is_out_cmd("<") == false);
    assert(is_out_cmd("|") == false);
    assert(is_out_cmd("command") == false);
}

// Test function for is_pipe_cmd
void test_is_pipe_cmd() {
    assert(is_pipe_cmd("|") == true);
    assert(is_pipe_cmd(">") == false);
    assert(is_pipe_cmd("<") == false);
    assert(is_pipe_cmd("command") == false);
}

// Test function for split
void test_split() {
    char *str = "ls -la | ws -l";
    int num_cmds;
    char **commands = split(str, PIPE_CMD, &num_cmds);
    if (commands == NULL)
        assert(false);

    for (int i = 0; i < num_cmds; i++) {
        trim(commands[i]);
        printf("cmd[%d] = %s\n", i, commands[i]);
    }

    assert(strcmp(commands[0], "ls -la") == 0);
    assert(strcmp(commands[1], "ws -l") == 0);

    for (int i = 0; i < num_cmds; i++) {
        free(commands[i]);
    }
    free(commands);
}

int main() {
    // Run tests
    test_is_in_cmd();
    test_is_out_cmd();
    test_is_pipe_cmd();
    test_execute();
    test_file_logging();
    test_split();

    printf("All tests passed!\n");

    return 0;
}
