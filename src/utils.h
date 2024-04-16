#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define ERROR_CODE -1
#define DELIMITER " "
#define PATH_DELIMITER ":"
#define IN_CMD "<"
#define OUT_CMD ">"
#define PIPE_CMD "|"
#define MAX_ARGS 64

/**
 * @brief verify if a string is a in command: '<'
 *
 * @param str string to verify
 * @return true if is '<' else false
 */
bool is_in_cmd(const char *str);

/**
 * @brief verify if a string is a out command: '>'
 *
 * @param str string to verify
 * @return true if is '>' else false
 */
bool is_out_cmd(const char *str);

/**
 * @brief verify if a string is a pipe: '>'
 *
 * @param str string to verify
 * @return true if is '|' else false
 */
bool is_pipe_cmd(const char *str);

/**
 * @brief execute a command
 *
 * @param argv all the orguments of the command
 * @param cmd the command
 * @param path the path envirioment variable
 * @param envp a list of envirioment variable
 */
void execute(char **argv, char *cmd, char *path, char *const *envp);

/**
 * @brief execute a command changing stdin and/or stdout
 *
 * @param argv all the orguments of the command
 * @param cmd the command
 * @param path the path envirioment variable
 * @param envp a list of envirioment variable
 * @param fd_in file descripter of new default input
 * @param fd_out file descripter of new default output
 */
void execute_cio(char **argv, char *cmd, char *path, char *const *envp, int fd_in, int fd_out);

/**
 * @brief loggin info into a file
 *
 * @param file pointer to file
 * @param severity 'i' for info, 'w' for warning and 'e' for error
 * @param info the information to be loged
 */
void file_logging(char *file_name, const char severity, const char *info);


/**
 * @brief wait for all forks
 *
 */
void wait_all();

/**
 * @brief split a string by 'ch' char into a array of string
 *
 * @param[in] str string to be splited
 * @param[in] ch delimiter
 * @param[out] count number of itens in the array
 * @return the arrys of string
 */
char** split(char *str, char* ch, int *count);

/**
 * @brief remove whitespaces from a string
 *
 * @param str string to remove whitespaces
 * @return cleaned string
 */
void trim(char *str);

/**
 * @brief get the PATH envirioment variable or get a default one
 *
 * @return path variable
 */
char* get_path();

/**
 * @brief get the env paramenters array based on path
 *
 * @param path env variable
 * @return envp array
 */
char** get_envp(char *path);

/**
 * @brief free an array of strings
 *
 * @param arr array to be freed
 */
void free_char_array(char **arr);
