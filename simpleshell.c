#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define PATH_MAX 4096
#define MAX_LENGTH 100

int  parseInput(char * input, char splitWords[][500], int maxWords);

void changeDirectories(const char * path);

int executeCommand(char * const* enteredCommand, const char* infile, const char* outfile);

int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}
// --------------------------------------------------------------------------------------------------------------------------
int main() {
    char cwd[PATH_MAX];
    char username[256];
    char input[MAX_LENGTH];
    char splitWords[MAX_LENGTH][500];

    char infile, outfile;

    while(1) {
        // Get username
        if (getlogin_r(username, sizeof(username)) != 0) {
            perror("getlogin_r");
            return 1;
        }
        printf("%s:", username);

        // Get working directory
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd() error");
            return 1;
        }
        printf("%s$ ", cwd);

        // Get user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;  // Exit on EOF
        }

        // Remove trailing newline
        input[strcspn(input, "\n")] = '\0';

        // Parse input
        int wordCount = parseInput(input, splitWords, 10);

        // Handle cd command
        if (wordCount > 0 && strcmp(splitWords[0], "cd") == 0) {
            if (wordCount == 2) {
                changeDirectories(splitWords[1]);
            } else {
                fprintf(stderr, "Path Not Formatted Correctly!\n");
            }
        }

        else {

            for (int i = 0; i < wordCount; i++) {
                if (*splitWords[i] == '>') {
                    outfile = *splitWords[i+1];
                }

                else if (*splitWords[i] == '<')
                {
                    infile = *splitWords[i+1];
                }
            }
            char** cArray;
            cArray = (char**)malloc(wordCount * sizeof(char*)) + 1;

            if (cArray == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }      

            for (int i = 0; i < wordCount; i++) {
                size_t len = strlen(splitWords[i]);
                cArray[i] = (char*)malloc((len + 1) * sizeof(char));

                if (cArray[i] == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");

                    for (int j = 0; j < i; j++) {
                        free(cArray[j]);
                    }

                    free(cArray);
                    return 1;
                }

            }

            executeCommand(cArray, &infile, &outfile);
        }
    }

    return 0;
}
// --------------------------------------------------------------------------------------------------------------------------
int parseInput(char * input, char splitWords[][500], int maxWords) {
    int counter = 0;
    char *start = input;
    char *end;
    int in_quotes = 0;
    char quote_char = '\0';

    while (*start && counter < maxWords) {
        // Skip leading whitespace
        while (is_whitespace(*start)) start++;
        if (*start == '\0') break;

        // Check for quotes
        if (*start == '"' || *start == '\'') {
            in_quotes = 1;
            quote_char = *start;
            start++;
        }

        // Find end of word or quoted string
        if (in_quotes) {
            end = strchr(start, quote_char);
            if (end == NULL) {
                // Unterminated quote, treat rest of string as one word
                end = start + strlen(start);
            }
        } else {
            end = start;
            while (*end && !is_whitespace(*end)) end++;
        }

        // Copy word to splitWords
        strncpy(splitWords[counter], start, end - start);
        splitWords[counter][end - start] = '\0';
        counter++;

        // Move start to next word
        start = (*end == '\0' || in_quotes) ? end : end + 1;
        in_quotes = 0;
    }

    return counter;
}
// --------------------------------------------------------------------------------------------------------------------------
int executeCommand(char * const* enteredCommand, const char* infile, const char* outfile) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork Failed: %s\n", strerror(errno));
        return 1;
    }
    if (pid == 0) {
        // CHILD PROCESS
        if (infile != NULL && *infile != '\0') {
            int fd = open(infile, O_RDONLY, 0666);
            if (fd == -1) {
                fprintf(stderr, "Failed to open input file: %s\n", strerror(errno));
                _exit(1);
            }
            if (dup2(fd, STDIN_FILENO) == -1) {
                fprintf(stderr, "Failed to redirect input: %s\n", strerror(errno));
                _exit(1);
            }
            close(fd);
        }

        if (outfile != NULL && *outfile != '\0') {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd == -1) {
                fprintf(stderr, "Failed to open output file: %s\n", strerror(errno));
                _exit(1);
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                fprintf(stderr, "Failed to redirect output: %s\n", strerror(errno));
                _exit(1);
            }
            close(fd);
        }

        execvp(enteredCommand[0], enteredCommand);
        
        // If execvp returns, it means an error occurred
        fprintf(stderr, "exec Failed: %s\n", strerror(errno));
        _exit(1);
    }
    else {
        // PARENT PROCESS
        if (wait(&status) == -1) {
            fprintf(stderr, "Wait failed: %s\n", strerror(errno));
            return 1;
        }

        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                fprintf(stderr, "Child finished with error status: %d\n", exit_status);
                return exit_status;
            }
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Child terminated by signal: %d\n", WTERMSIG(status));
            return 1;
        }
    }

    return 0;  // Success
}
// --------------------------------------------------------------------------------------------------------------------------
void changeDirectories(const char* path) {
    if (path == NULL || *path == '\0') {
        fprintf(stderr, "Path Not Formatted Correctly!\n");
        return;
    }

    if (chdir(path) != 0) {
        fprintf(stderr, "chdir Failed: %s\n", strerror(errno));
    }
}
// --------------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------------