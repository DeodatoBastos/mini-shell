#include "../src/utils.h"
#include <assert.h>

int test_file_logging() {
    FILE* file = fopen("logs/test.txt", "a");

    file_logging(file, 'i', "test");
    file_logging(file, 'w', "test");
    file_logging(file, 'e', "test");

    fclose(file);

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

// Test function for execute - Cannot be tested in isolation without actual implementation

// Test function for file_logging - Cannot be tested in isolation without actual implementation

int main() {
    // Run tests
    test_is_in_cmd();
    test_is_out_cmd();
    test_is_pipe_cmd();
    test_execute();
    test_file_logging();

    printf("All tests passed!\n");

    return 0;
}
