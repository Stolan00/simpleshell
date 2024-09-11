#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH_MAX 4096

int main() {
    char cwd[PATH_MAX];

    char username[256];

    if (!getlogin_r(username, sizeof(username))) {
        printf("%s:", username);
    }

    else {
        perror("getlogin_r");
        return 1;
    }

    if (getcwd(cwd, sizeof(cwd))) {
        printf("%s$\n", cwd);
    }

    else {
        perror("getcwd() error");
        return 1;
    }
}

