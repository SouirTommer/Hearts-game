#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

int getRankIndex(char rank, int mode)
{
    char smallRanks[] = "23456789TJQKA";
    char highRanks[] = "AKQJT98765432";

    if (mode == 0)
    {
        for (int i = 0; i < 13; i++)
        {
            if (smallRanks[i] == rank)
            {
                return i;
            }
        }
    }
    else
    {
        for (int i = 0; i < 13; i++)
        {
            if (highRanks[i] == rank)
            {
                return i;
            }
        }
    }

    return -1;
}

// get small suit
int getSuitIndex(char suit, int mode)
{
    char smallSuits[] = "DCHS";
    char highSuits[] = "SHCD";

    if (mode == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (smallSuits[i] == suit)
            {
                return i;
            }
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            if (highSuits[i] == suit)
            {
                return i;
            }
        }
    }

    return -1;
}

// find smallest card
void findSmallCard(int i, char playercard[13][3], char smallestCard[3])
{
    // find the smallest card
    int j;
    int smallestRank = 13;
    int smallestSuit = 4;
    int suitIndex;

    for (j = 0; j < 13; j++)
    {
        int rank = getRankIndex(playercard[j][i + 1], 0);
        int suit = getSuitIndex(playercard[j][i], 0);
        if (rank < smallestRank || (rank == smallestRank && suit < smallestSuit))
        {
            smallestSuit = suit;
            smallestRank = rank;
            smallestCard[0] = playercard[j][i];
            smallestCard[1] = playercard[j][i + 1];
            smallestCard[2] = '\0';
        }
    }
}

void sortTheCardSuit(int i, int fdAssignCard[4][2], char playercard[13][3])
{
    int j;
    int k;
    char temp;

    // Bubble sort by suit S>H>C>D
    for (j = 0; j < 13; j++)
    {
        for (k = 0; k < 13 - j - 1; k++)
        {

            if (getRankIndex(playercard[k][i + 1], 1) > getRankIndex(playercard[k + 1][i + 1], 1))
            {
                // Swap cards
                temp = playercard[k][i];
                playercard[k][i] = playercard[k + 1][i];
                playercard[k + 1][i] = temp;

                temp = playercard[k][i + 1];
                playercard[k][i + 1] = playercard[k + 1][i + 1];
                playercard[k + 1][i + 1] = temp;
            }
            if (getSuitIndex(playercard[k][i], 1) > getSuitIndex(playercard[k + 1][i], 1))
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

void readAssignedCards(int i, int fdAssignCard[4][2], char playercard[13][3])
{
    int j;

    printf("Child %d, pid %d: received ", i + 1, getpid());
    for (j = 0; j < 13; j++)
    {
        read(fdAssignCard[i][0], &playercard[j][i], 3);
        printf("%s ", &playercard[j][i]); // Print the value of child[j][3] instead of child[j]
    }
    printf("\n");

    close(fdAssignCard[i][0]);
}

void assignCards(char *cards[52], int fdAssignCard[4][2])
{
    int i;
    int s;

    for (i = 0; i < 4; i++)
    {
        close(fdAssignCard[i][0]);
    }

    for (i = 0; i < 52; i++)
    {
        write(fdAssignCard[i % 4][1], cards[i], 3);
    }

    for (i = 0; i < 4; i++)
    {
        close(fdAssignCard[i][1]);
    }
}

void readFile(int i, char *cards[52], char *argv[])
{

    FILE *file = fopen(argv[1], "r");
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

int main(int argc, char *argv[])
{
    int i, j;
    char childcard[13][3];
    char *cards[52];

    int childNo;
    int processPid;
    char playCard[3];
    char sentCard[52];

    // fd
    int fd[2];
    int fdAssignCard[4][2];
    int fdParentToChild[4][2];
    int fdChildToParent[4][2];
    char buf[80];

    readFile(i, cards, argv); // Read the file

    pid_t pid;

    if (pipe(fd) < 0)
    {
        printf("Pipe creation error\n");
        exit(1);
    }
    for (i = 0; i < 4; i++)
    {
        if (pipe(fdAssignCard[i]) < 0)
        {
            printf("Pipe creation error1\n");
            exit(1);
        }
        if (pipe(fdParentToChild[i]) < 0)
        {
            printf("Pipe creation error2\n");
            exit(1);
        }
        if (pipe(fdChildToParent[i]) < 0)
        {
            printf("Pipe creation error3\n");
            exit(1);
        }
    }

    //////////////////////// Child Process ////////////////////////
    //////////////////////// Child Process ////////////////////////
    for (i = 0; i < 4; i++)
    {
        char childReceiveBuffer[80];
        char childSendBuffer[80];

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

            readAssignedCards(i, fdAssignCard, childcard);
            sortTheCardSuit(i, fdAssignCard, childcard);
            for (j = 0; j < 4; j++)
            {
                close(fdAssignCard[j][0]);
            }

            char wtemp[10] = "H3";
            char rtemp[10];
            ssize_t numBytes;

            while ((numBytes = read(pipes_to_child[i][0], rtemp, sizeof(rtemp))) > 0)
            {
                printf("child read \n");
                write(pipes_from_child[i][1], wtemp, sizeof(wtemp));
                numBytes = 0;

                memset(rtemp, 0, sizeof(rtemp));
            }

            // ssize_t bytesLength;
            // char smallestCard[3];
            // while ((bytesLength = read(fdParentToChild[i][0], childReceiveBuffer, sizeof(childReceiveBuffer))) > 0)
            // {

            //     printf("%zd", bytesLength);
            //     // printf("Child %d, pid %d: play ", i + 1, getpid());
            //     findSmallCard(i, childcard, smallestCard);

            //     printf("%s", smallestCard);
            //     strcpy(sentCard, smallestCard);

            //     // bytesLength = 0;
            //     // memset(&childReceiveBuffer, 0, sizeof(childReceiveBuffer));

            //     // write(fdChildToParent[i][1], &sentCard, sizeof(sentCard));
            // }

            return 0;
        }
    }

    //////////////////////// Parent Process ////////////////////////
    //////////////////////// Parent Process ////////////////////////

    if (pid > 0)
    {

        int round = 1;
        int currentPlayer = 0, currentmark = 0;
        int winner;
        char sendBuffer[80];
        char receiveBuffer[80];

        printf("Parent pid %d: child players are", getpid());
        for (i = 0; i < 4; i++)
        {
            read(fd[0], &processPid, sizeof(processPid));
            printf(" %d", processPid);
        }
        printf("\n");

        assignCards(cards, fdAssignCard);

        for (i = 0; i < 4; i++)
        {
            close(fdAssignCard[i][0]);
            close(fdAssignCard[i][1]);
        }

        int winnerIndex = 0;
        char wtemp[10] = "write";
        char rtemp[10] = "read";
        sleep(1);
        for (i = 0; i < 1; i++)
        {

            for (currentPlayer = 0; currentPlayer < 4; currentPlayer++)
            {

                for (int i = 1; i <= 13 ; i++)
                {
                    printf("Parent pid %d: round %d child %d to lead", getpid(), i, winnerIndex + 1);
                    printf("\n");
                    for (int j = 0; j < 4; j++)
                    {
                        write(pipes_to_child[j][1], &wtemp, sizeof(wtemp));
                        read(pipes_from_child[j][0], &rtemp, sizeof(rtemp));
                        printf("%s \n", rtemp);
                    }
                }
                // if(i == 0 && currentPlayer == 0){
                //     strcpy(sendBuffer, "startgame");
                // }
                //     write(fdParentToChild[currentPlayer][1], sendBuffer, sizeof(sendBuffer));

                // if(i == 0 && currentPlayer == 0){
                //     memset(&sendBuffer, 0, sizeof(sendBuffer));
                // }
                //     sleep(1);

                //     read(fdChildToParent[currentPlayer][0], receiveBuffer, sizeof(receiveBuffer));

                //     // sentCard[currentPlayer] = receiveBuffer;
                //     // sendBuffer = sentCard[currentPlayer];

                //     // write(fdParentToChild[currentPlayer][1], sendBuffer, sizeof(sendBuffer));

                //     printf("Parent pid %d: round %d child %d to lead\n", getpid(), round, currentPlayer + 1);
            }
            sleep(1);

            // print buffer
        }
        printf("endgame");
    }

    for (i = 0; i < 4; i++)
    {
        wait(NULL);
    }
    return 0;
}
