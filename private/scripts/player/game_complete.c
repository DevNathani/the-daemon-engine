#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include "db.h"
#include <pwd.h>

#define TROPHY_FILE "/opt/game/assets/trophy.txt"

/* ANSI escape codes */
#define CLR_SCREEN "\033[2J\033[H"
#define GREEN  "\033[32m"
#define WHITE  "\033[97m"
#define GRAY   "\033[90m"
#define BOLD   "\033[1m"
#define RESET  "\033[0m"
#define CYAN   "\033[36m"
#define YELLOW "\033[33m"
#define PURPLE "\033[35m"

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

void print_message(const char *hint) {
    print_interpreted(hint);
    putchar('\n');
}

/* get score from DB */
int db_get_score(sqlite3 *db, const char *user) {
    sqlite3_stmt *stmt = db_prepare(db,
        "SELECT score FROM progress WHERE username=?"
    );

    if (!stmt) return 0;

    int score = 0;

    sqlite3_bind_text(stmt, 1, user, -1, SQLITE_STATIC);

    if (db_step(stmt) == SQLITE_ROW) {
        score = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return score;
}

const char* get_current_user() {
    uid_t uid = getuid();                 // real user
    struct passwd *pw = getpwuid(uid);
    if (!pw) return NULL;
    return pw->pw_name;
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: game_complete <username>\n");
        return 1;
    }

    const char *user = argv[1];

    const char *real_user = get_current_user();
    if (!real_user || strcmp(user, real_user) != 0) {
        fprintf(stderr, "Unauthorized access\n");
    return 1;
}
    const char *event = "theDaemon";

    setvbuf(stdout, NULL, _IONBF, 0);

    printf(CLR_SCREEN);

    /* Trophy */
    FILE *fp = fopen(TROPHY_FILE, "r");
    if (!fp) {
        fprintf(stderr, "ERROR: trophy file (%s): %s\n",
                TROPHY_FILE, strerror(errno));
    } else {
        char line[512];
        printf("\n");
        while (fgets(line, sizeof(line), fp)) {
            printf("   %s%s%s", YELLOW, line, RESET);
        }
        fclose(fp);
        printf("\n");
    }

    /* Time */
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);

    char date[64] = "unknown";
    if (tm)
        strftime(date, sizeof(date), "%d %b %Y, %H:%M", tm);

    /* Score from DB */
    sqlite3 *db = db_open();
    if (!db) {
        fprintf(stderr, "DB error\n");
        return 1;
    }

    int score = db_get_score(db, user);
    db_close(db);

    /* Output */
    printf(BOLD GREEN "CONGRATULATIONS, %s!" RESET "\n\n", user);

    printf(WHITE "You Have Completed the Game " RESET
           BOLD "%s" RESET "\n", event);

    printf(WHITE "This required curiosity, patience, and real Linux skills." RESET "\n\n");

    printf(CYAN "Player       : " RESET "%s\n", user);
    printf(CYAN "Score        : " RESET BOLD "%d" RESET "\n", score);
    printf(CYAN "Event        : " RESET PURPLE "%s" RESET "\n", event);
    printf(CYAN "DEVELOPER    : " RESET YELLOW "DEV NATHANI" RESET "\n");
    printf(CYAN "Time         : " RESET "%s\n\n", date);

    printf(WHITE "🏁 Your journey ends here. The system is now locked." RESET "\n\n");
    printf(GRAY "You may logout safely." RESET "\n\n");

    sleep(5);
    return 0;
}