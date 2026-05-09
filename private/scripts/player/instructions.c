#include <stdio.h>
#include <unistd.h>

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define CYAN    "\033[36m"
#define YELLOW  "\033[33m"

int main() {

    printf("\033[2J\033[H"); // clear screen

    printf(GREEN "Initializing Daemon interface...\n\n" RESET);

    sleep(1);

    printf(CYAN "--------------------------------------------------\n\n" RESET);

    printf(CYAN "Objective:\n" RESET);
    printf("Locate the hidden key in each level and execute:\n\n");

    printf("    " RED "$ unlock" RESET "\n\n");

    sleep(1);

    printf(CYAN "--------------------------------------------------\n\n" RESET);

    printf(CYAN "Rules enforced by the Daemon:\n" RESET);
   printf("\t- You are free to explore the entire system, but the key will always be hidden within your home directory (~/<user>).\n");
    printf("\t- Play fair\n");
    printf("\t- Enjoy the learnings\n");
    printf("\t- Do not interfere with other users or the system\n\n");

    sleep(1);

    printf(CYAN "--------------------------------------------------\n\n" RESET);

    printf(CYAN "Available Commands:\n" RESET);
    printf("\t- " RED "unlock" RESET "      -> submit your key\n");
    printf("\t- " RED "giveup" RESET "      -> reveal current level final hint (0 points)\n");
    printf("\t- " RED "reload" RESET "      -> reload current level\n\n");

    sleep(1);

    printf(CYAN "--------------------------------------------------\n\n" RESET);

    printf(CYAN "Scoring Protocol:\n" RESET);
    printf("\t- Points per level = (level + 1) * 5\n");
    printf("\t- 0 points if you give up\n\n");

    sleep(1);

    printf(CYAN "--------------------------------------------------\n\n" RESET);

    printf(CYAN "To inspect any command, use:\n" RESET);
    printf("\t" RED "--help" RESET "   or   " RED "man <command>" RESET "\n\n");

    printf(CYAN "--------------------------------------------------\n\n" RESET);

    sleep(1);

    printf(YELLOW "Notice:\n" RESET);
    printf(YELLOW "\tNothing is given.\n");
    printf("\tEverything is placed.\n\n");

    printf("\tOnly those who search... will find.\n\n" RESET);

    return 0;
}