#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX 4096
#define MAX_LENGTH 100

int  parseInput(char * input, char splitWords[][500], int maxWords);

void changeDirectories(const char * path);

int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}
// --------------------------------------------------------------------------------------------------------------------------
int main() {
    char cwd[PATH_MAX];
    char username[256];
    char input[MAX_LENGTH];
    char splitWords[MAX_LENGTH][500];

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

            changeDirectories(splitWords[1]);
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

// --------------------------------------------------------------------------------------------------------------------------
void changeDirectories(const char* path) {
    if (chdir(path) != 0) {
        perror("chdir() error");
    }
}
// --------------------------------------------------------------------------------------------------------------------------