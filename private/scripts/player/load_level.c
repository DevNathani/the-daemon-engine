#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "db.h"
#include <sys/types.h>
#include <pwd.h>

#define MAX 512
#define CLR_SCREEN "\033[2J\033[3J\033[H"
#define GREEN  "\033[32m"
#define WHITE  "\033[97m"
#define GRAY   "\033[90m"
#define BOLD   "\033[1m"
#define RESET  "\033[0m"
#define CYAN   "\033[36m"
#define YELLOW "\033[33m"
#define PURPLE "\033[35m"
#define MAGENTA "\e[35m"
#define RED "\033[1;31m"

const char* get_current_user() {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (!pw) return NULL;
    return pw->pw_name;
}

void print_interpreted(const char *s) {
    while (*s) {
        if (*s == '\\') {
            s++;
            switch (*s) {
                case 'n': putchar('\n'); break;
                case 't': putchar('\t'); break;
                case '\\': putchar('\\'); break;
                default:
                    putchar('\\');
                    putchar(*s);
            }
        } else {
            putchar(*s);
        }
        s++;
    }
}

void print_submit(const char *hint) {
    print_interpreted(hint);
    putchar('\n');
}
// show message from DB
void show_previous_level_message(sqlite3 *db, const char *user)
{
    int level = db_get_level(db, user);
    int prev = level - 1;

    if (prev < 0) return;

    const char *msg = db_get_level_message(db, prev);
    if (!msg) return;

    printf("\n----------------------------------------\n");
    printf("\033[33m");
    print_submit(msg);
    printf("\033[0m\n");
    printf("----------------------------------------\n\n");
}

// ASCII banner
void banner(const char *user) {
    printf(GREEN);
    printf("\n");
    printf("=============================================\n");
    printf("        SUBJECT" BOLD " %s\n" RESET GREEN ,  user);
    printf("     PASSED ALL THE TESTS\n");
    printf("=============================================\n\n");
    printf(RESET);
}

int main(int argc, char **argv)
{
    if (argc < 2) return 1;

    char *user = argv[1];

    const char *real_user = get_current_user();

    if (!real_user || strcmp(user, real_user) != 0) {
        fprintf(stderr, "Unauthorized access\n");
        return 1;
    }
    
    printf(CLR_SCREEN);
    fflush(stdout);
    int just_solved = 0;
    if (argc >= 3)
        just_solved = atoi(argv[2]);

    char src[MAX], dst[MAX];

    snprintf(dst, sizeof(dst), "/home/%s", user);

    sqlite3 *db = db_open();
    if (!db) return 1;

    // fetch state
    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT level, completed FROM progress WHERE username=?"
    );

    int level = 0, completed = 0;

    if (stmt) {
        sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);

        if (db_step(stmt) == SQLITE_ROW) {
            level     = sqlite3_column_int(stmt, 0);
            completed = sqlite3_column_int(stmt, 1);
        }

        sqlite3_finalize(stmt);
    }

    // completed check
    if (completed == 1) {
        db_close(db);
        execl("/opt/game/bin/player/game_complete",
              "game_complete", user, NULL);
        return 0;
    }

    // DISPLAY PREVIOUS SUBMIT MESSAGE
    if(just_solved){
        show_previous_level_message(db, user);
    }

    int max = db_get_max_level(db);
    if (max < 0) {
        db_close(db);
        return 1;
    }
    if (level > max) {
        db_close(db);
        execl("/opt/game/bin/player/game_complete",
              "game_complete", user, NULL);
        return 0;
    }

    snprintf(src, sizeof(src),
             "/opt/game/levels/level%d/", level);

    db_close(db);

    // -------- SYNC LEVEL (NO DELETE FIRST) --------
    if (fork() == 0) {
        execlp("rsync", "rsync",
               "-rltD",
               "--delete",        // cleans old files safely
               "--no-links",
               "--exclude=hash",
               src, dst,
               NULL);
        _exit(1);
    }
    wait(NULL);

    // -------- FIX OWNERSHIP --------
    if (fork() == 0) {
        char usergrp[128];
        snprintf(usergrp, sizeof(usergrp), "%s:%s", user, user);

        execlp("chown", "chown",
               "-R", usergrp, dst,
               NULL);
        _exit(1);
    }
    wait(NULL);

    chmod(dst, 0700);

    // -------- SHOW SCENE --------
    char scene[MAX];
    snprintf(scene, sizeof(scene), "%s/SCENE", dst);

    FILE *rf = fopen(scene, "r");
    if (rf) {
        char line[256];
        printf("\n");
        while (fgets(line, sizeof(line), rf)) {
            printf("%s", line);
        }
        printf("\n");
        fclose(rf);
    }

    return 0;
}