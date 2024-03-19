
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

int main()
{
    int i = 0;
    char *cards[52];

    FILE *file = fopen("card1.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }
    char card[3];
    while (fscanf(file, "%2s", card) == 1) {
        strcat(cards[i], card);
        i++;
    }
    fclose(file);
    printf("%s\n", *cards);

}