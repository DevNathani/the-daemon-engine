#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "db.h"

#define RED   "\033[31m"
#define RESET "\033[0m"
#define BOLD   "\033[1m"

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

void print_hint(const char *hint) {
    print_interpreted(hint);
    putchar('\n');
}



int main() {

    char *user = getlogin();
    if (!user) user = getenv("USER");
    if (!user) return 1;

    char choice;
    printf(RED BOLD "\nAre you sure you want to run this? (y/n): " RESET);
    scanf(" %c", &choice);

    if(choice == 'y' || choice == 'Y'){

        sqlite3 *db = db_open();
        if (!db) return 1;

    // get current level
    int level = db_get_level(db, user);

    // get level name
    const char *level_name = db_get_level_name(db, level);

    // get final hint
    const char *hint = db_get_final_hint(db, level);
    
    printf("\n========================================\n");
    printf(" GIVEUP ACTIVATED \n");
    printf("========================================\n");
    
    printf("Level : %d\n", level);
    printf("Room  : %s\n", level_name ? level_name : "Unknown");
    
    printf("----------------------------------------\n");
    
    // print hint
    if (hint && strlen(hint) > 0) {
        print_hint(hint);
    } else {
        printf("No hint available for this level.\n");
    }
    
    printf("----------------------------------------\n");
    
    // warning
    printf(RED "You will receive 0 points for this level\n" RESET);
    
    printf("========================================\n\n");
    
    // mark giveup
    db_set_giveup(db, user);
    
    // log submission
    db_insert_submission(db, user, level, 1, 0);
    
    db_close(db);
    }
    else if(choice == 'n' || choice == 'N'){
        printf("Aborted\n");
    }
    else{
        printf("No Valid Input Found\nAborting\n");
    }
    return 0;
}