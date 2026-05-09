#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(void) {

    // get real user
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (!pw || !pw->pw_name) {
        fprintf(stderr, "reload_level: cannot determine user\n");
        return 1;
    }

    const char *user = pw->pw_name;

    // reload environment
    execl("/opt/game/bin/player/load_level",
          "load_level",
          user,
          NULL);

    perror("reload_level: failed to exec load_level");
    return 1;
}