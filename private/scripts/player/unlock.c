#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

#define CHECK "/opt/game/bin/player/check_and_advance"
#define LOAD  "/opt/game/bin/player/load_level"
#define MAXP 256


char* trim(char *str) {
    char *end;
    // Trim leading spaces
    while (isspace((unsigned char)*str)) {
        str++;
    }
    // If all spaces
    if (*str == '\0') {
        return str;
    }
    // Trim trailing spaces
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    return str;
}

int main()
{
    char pass[MAXP];

    //  get username
    char *user = getlogin();
    if (!user) user = getenv("USER");
    if (!user) {
        fprintf(stderr, "Error: unable to determine user\n");
        return 1;
    }

    //  input password/key
    printf("Enter Key: ");
    fflush(stdout);

    if (!fgets(pass, sizeof(pass), stdin))
        return 1;

    pass[strcspn(pass, "\n")] = 0;   // remove newline
    char *clean = trim(pass);        // trim spaces

    //  create pipe
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    //  execute check_and_advance (SUID root)
    if (pid == 0)
    {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);

        execl(CHECK, "check_and_advance", user, NULL);
        perror("exec check_and_advance");
        _exit(1);
    }

    //  PARENT → send input
    close(fd[0]);

    write(fd[1], clean, strlen(pass));
    write(fd[1], "\n", 1);
    close(fd[1]);

    int status;
    waitpid(pid, &status, 0);

    //  SUCCESS → load next level
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        if (fork() == 0)
        {
            execl(LOAD, "load_level", user,"1", NULL);
            perror("exec load_level");
            _exit(1);
        }
        wait(NULL);
    }
    else
    {
        printf(" Wrong key\n");
    }

    return 0;
}