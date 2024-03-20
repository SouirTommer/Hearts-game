#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

int getSuitIndex(char suit)
{
    char suits[] = "SHCD";
    for (int i = 0; i < 4; i++)
    {
        if (suits[i] == suit)
        {
            return i;
        }
    }
    return -1;
}
int getBigrank(char rank)
{
    char ranks[] = "AKQJT98765432";
    for (int i = 0; i < 13; i++)
    {
        if (ranks[i] == rank)
        {
            return i;
        }
    }
    return -1;
}

void sortTheCardSuit(int i, int fd2[4][2], char playercard[13][3])
{
    int j;
    int k;
    char temp;

    // Bubble sort by suit S>H>C>D
    for (j = 0; j < 13; j++)
    {
        for (k = 0; k < 13 - j - 1; k++)
        {

            if (getBigrank(playercard[k][i + 1]) > getBigrank(playercard[k + 1][i + 1]))
            {
                // Swap cards
                temp = playercard[k][i];
                playercard[k][i] = playercard[k + 1][i];
                playercard[k + 1][i] = temp;

                temp = playercard[k][i + 1];
                playercard[k][i + 1] = playercard[k + 1][i + 1];
                playercard[k + 1][i + 1] = temp;
            }
            if (getSuitIndex(playercard[k][i]) > getSuitIndex(playercard[k + 1][i]))
            {
                // Swap cards
                temp = playercard[k][i];
                playercard[k][i] = playercard[k + 1][i];
                playercard[k + 1][i] = temp;

                temp = playercard[k][i + 1];
                playercard[k][i + 1] = playercard[k + 1][i + 1];
                playercard[k + 1][i + 1] = temp;
            }
        }
    }

    printf("Child %d, pid %d: arranged ", i + 1, getpid());
    // Print the sorted array
    for (j = 0; j < 13; j++)
    {
        printf("%c%c ", playercard[j][i], playercard[j][i + 1]);
    }
    printf("\n");
}

void readAssignedCards(int i, int fd2[4][2], char playercard[13][3])
{
    int j;

    printf("Child %d, pid %d: received ", i + 1, getpid());
    for (j = 0; j < 13; j++)
    {
        read(fd2[i][0], &playercard[j][i], 3);
        printf("%s ", &playercard[j][i]); // Print the value of child[j][3] instead of child[j]
    }
    printf("\n");
}

void assignCards(char *cards[52], int fd2[4][2])
{
    int i;
    int s;

    for (i = 0; i < 4; i++)
    {
        close(fd2[i][0]);
    }

    for (i = 0; i < 52; i++)
    {
        write(fd2[i % 4][1], cards[i], 3);
    }

    for (i = 0; i < 4; i++)
    {
        close(fd2[i][1]);
    }
}

void readFile(int i, char *cards[52])
{
    FILE *file = fopen("card1.txt", "r");
    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        exit(1);
    }
    char card[3];
    while (fscanf(file, "%2s", card) == 1)
    {
        cards[i] = malloc(3 * sizeof(char));
        strcpy(cards[i], card);
        i++;
    }
    fclose(file);
}

int main()
{
    int fd[2];
    int fd2[4][2];
    int i = 0;
    int j;
    char childcard[13][3];
    char *cards[52];
    int childNo;
    pid_t pid;

    readFile(i, cards); // Read the file

    if (pipe(fd) < 0)
    {
        printf("Pipe creation error\n");
        exit(1);
    }
    for (i = 0; i < 4; i++)
    {
        if (pipe(fd2[i]) < 0)
        {
            printf("Pipe creation error\n");
            exit(1);
        }
    }

    //////////////////////// Child Process ////////////////////////
    //////////////////////// Child Process ////////////////////////
    for (i = 0; i < 4; i++)
    {
        childNo = i + 1;
        pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Fork failed\n");
            return 1;
        }
        else if (pid == 0)
        {
            int currentPid = getpid();
            close(fd[0]);
            write(fd[1], &currentPid, sizeof(currentPid));
            close(fd[1]);

            close(fd2[i][1]);

            readAssignedCards(i, fd2, childcard);
            sortTheCardSuit(i, fd2, childcard);
            for (j = 0; j < 4; j++)
            {
                close(fd2[j][0]);
            }
            return 0;
        }
    }

    //////////////////////// Parent Process ////////////////////////
    //////////////////////// Parent Process ////////////////////////
    printf("Parent pid %d: child players are", getpid());
    for (i = 0; i < 4; i++)
    {
        read(fd[0], &childNo, sizeof(childNo));
        printf(" %d", childNo);
    }
    printf("\n");

    assignCards(cards, fd2);

    for (i = 0; i < 4; i++)
    {
        wait(NULL);
    }
    return 0;
}
