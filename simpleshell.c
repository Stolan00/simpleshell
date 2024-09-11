#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX 4096
#define MAX_LENGTH 100

int main() {
    char cwd[PATH_MAX];
    char username[256];
    char input[MAX_LENGTH];

    // get username
    if (!getlogin_r(username, sizeof(username))) {
        printf("%s:", username);
    }

    else {
        perror("getlogin_r");
        return 1;
    }

    // get working directory
    if (getcwd(cwd, sizeof(cwd))) {
        printf("%s$ ", cwd);
    }

    else {
        perror("getcwd() error");
        return 1;
    }

    // get user input
    if (fgets(input, sizeof(input), stdin) != NULL) {
        // Remove trailing newline if present
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
        }
    }
}

