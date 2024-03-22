#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

int getNumericRank(char rank){
    switch (rank){
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'T':
        return 10;
    case 'J':
        return 11;
    case 'Q':
        return 12;
    case 'K':
        return 13;
    case 'A':
        return 14;
    default:
        return -1;
    }
}
int calScore(char *sendBuffer, int score[4], int winner){
    int i = 0;
    int j = 0;
    int temp = 0;
    int tempScore = 0;
    for (i = 0; i < 11; i += 1){
        if (sendBuffer[i] == 'H'){
            tempScore += 1;
        }
        // For SQ card, +15
        if (sendBuffer[i] == 'S' && sendBuffer[i + 1] == 'Q'){
            tempScore += 13;
        }
    }
    score[winner] += tempScore;
    // if score is 26, then set other to 0, itself to 26
    int found26 = 0;
    for (i = 0; i < 4; i += 1){
        if (score[i] == 26){
            score[i] = 0;
            temp = i;
            found26 = 1;
        }
    }

    if (found26){
        for (i = 0; i < 4; i += 1){
            if (score[i] == 0 && i != temp){
                score[i] = 26;
            }
        }
    }

    // printf("Score = <%d %d %d %d>\n", score[0], score[1], score[2], score[3]);

    return score[winner];
}

// check the winner of the trick
int checkWin(char *sendBuffer, int playerOrder[4]){
    int i = 0;
    int winner = 0;

    int currentMaxRank = getNumericRank(sendBuffer[1]);
    int currentSuit = sendBuffer[0];

    for (i = 1; i < 11; i += 3){

        int nextRank = getNumericRank(sendBuffer[i + 3]);
        char nextSuit = sendBuffer[i + 2];
        // check the player suit, if decard, then skip
        if (nextSuit != currentSuit){
            continue;
        }

        if (nextRank > currentMaxRank){
            currentMaxRank = nextRank; // update the max rank
            winner = (i + 3) / 3; // because the i is 1, 4, 7, 10, so we need to divide by 3 to get the player number
        }
    }

    winner = playerOrder[winner];
    printf("Parent pid %d: Child %d wins the trick\n", getpid(), winner + 1);
    return winner;
}

int getRankIndex(char rank, int mode)
{
    char smallRanks[] = "23456789TJQKA"; // two mode to find
    char highRanks[] = "AKQJT98765432";
    int i;

    if (mode == 0){
        for (i = 0; i < 13; i++){
            if (smallRanks[i] == rank){
                return i;
            }
        }
    } else{
        for (i = 0; i < 13; i++){
            if (highRanks[i] == rank){
                return i;
            }
        }
    }

    return -1;
}

// get small suit
int getSuitIndex(char suit, int mode){
    char smallSuits[] = "DCHS";
    char highSuits[] = "SHCD";
    int i;

    if (mode == 0){
        for (i = 0; i < 4; i++){
            if (smallSuits[i] == suit){
                return i;
            }
        }
    }
    else{
        for (i = 0; i < 4; i++){
            if (highSuits[i] == suit){
                return i;
            }
        }
    }

    return -1;
}

int isCardInPlayRecord(char suit, char rank, char *playedCardRecord){
    int i;
    for (i = 0; playedCardRecord[i] != '\0'; i += 3){
        if ((playedCardRecord[i] == suit) && (playedCardRecord[i + 1] == rank)){ 
            //if the card is in the playedCardRecord then return 1
            return 1;
        }
    }
    return 0;
}

// find the play card for the player
//!!! most important function
void findPlayCard(int i, char playercard[13][3], char playCard[3], char *receiveBuffer, char *playedCardRecord){
    int j, k;
    int smallestRank = 13;
    int smallestSuit = 4;
    int biggestRank = 13;
    int biggestSuit = 4;
    int suitIndex;

    char suit = receiveBuffer[0];

    // if last card is played, then play the card
    if (suit == 'H' || suit == 'C' || suit == 'D' || suit == 'S'){
        // this for loop is to find the smallest card
        for (j = 0; j < 13; j++){
            char card[3] = {playercard[j][i], playercard[j][i + 1], '\0'};
            int rankIndex = getRankIndex(playercard[j][i + 1], 0);

            if ((rankIndex < smallestRank && 
                suit == playercard[j][i]) && 
                (!isCardInPlayRecord(suit, playercard[j][i + 1], playedCardRecord))){
                smallestRank = rankIndex;
                playCard[0] = playercard[j][i];
                playCard[1] = playercard[j][i + 1];
                playCard[2] = '\0';
            }
        }
        // if the player has the SQ card, then play it
        if (playCard[0] == '\0'){
            for (j = 0; j < 13; j++){
                int rankIndex = getRankIndex(playercard[j][i + 1], 1);
                int suitIndex = getSuitIndex(playercard[j][i], 1);
    
                if (((playercard[j][i] == 'S') &&
                     (playercard[j][i+1] == 'Q')) &&
                     (!isCardInPlayRecord(playercard[j][i], playercard[j][i + 1], playedCardRecord))){
                        playCard[0] = playercard[j][i];
                        playCard[1] = playercard[j][i + 1];
                        playCard[2] = '\0';
                        break;
                        
                    // if the player have highest H card, then play it
                } else if ((rankIndex < biggestRank || 
                    (rankIndex == biggestRank)) &&
                    playercard[j][i] == 'H' && 
                    (!isCardInPlayRecord(playercard[j][i], playercard[j][i + 1], playedCardRecord))){
                        // printf("player have H card so play H card first\n");
                        biggestRank = rankIndex;
                        playCard[0] = playercard[j][i];
                        playCard[1] = playercard[j][i + 1];
                        playCard[2] = '\0';
                        break;
                } else{
                    // if the player has the highest card, then play it
                    if ((rankIndex < biggestRank || 
                    (rankIndex == biggestRank && 
                    suitIndex < biggestSuit)) && 
                    (!isCardInPlayRecord(playercard[j][i], playercard[j][i + 1], playedCardRecord))){
                        biggestSuit = suitIndex;
                        biggestRank = rankIndex;
                        playCard[0] = playercard[j][i];
                        playCard[1] = playercard[j][i + 1];
                        playCard[2] = '\0';
                    }
                }
            }
        }
        // for each round fist card, the player should play the biggest card
    } else{
        for (j = 0; j < 13; j++){
            int rankIndex = getRankIndex(playercard[j][i + 1], 0);
            int suitIndex = getSuitIndex(playercard[j][i], 0);
            if ((rankIndex < biggestRank || 
            (rankIndex == biggestRank && suitIndex < biggestSuit)) && 
            (!isCardInPlayRecord(playercard[j][i], playercard[j][i + 1], playedCardRecord))){
                biggestSuit = suitIndex;
                biggestRank = rankIndex;
                playCard[0] = playercard[j][i];
                playCard[1] = playercard[j][i + 1];
                playCard[2] = '\0';
            }
        }
    }
}
//sort the card suit when assign card to the player
void sortTheCardSuit(int i, int fdAssignCard[4][2], char playercard[13][3]){
    int j;
    int k;
    char temp;

    for (j = 0; j < 13; j++){
        for (k = 0; k < 13 - j - 1; k++){
            // if the rank is bigger, then swap
            if (getRankIndex(playercard[k][i + 1], 1) > getRankIndex(playercard[k + 1][i + 1], 1)){
                temp = playercard[k][i];
                playercard[k][i] = playercard[k + 1][i];
                playercard[k + 1][i] = temp;

                temp = playercard[k][i + 1];
                playercard[k][i + 1] = playercard[k + 1][i + 1];
                playercard[k + 1][i + 1] = temp;
            }
            if (getSuitIndex(playercard[k][i], 1) > getSuitIndex(playercard[k + 1][i], 1)){
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
    for (j = 0; j < 13; j++){
        printf("%c%c ", playercard[j][i], playercard[j][i + 1]);
    }
    printf("\n");
}

void readAssignedCards(int i, int fdAssignCard[4][2], char playercard[13][3]){
    int j;
    printf("Child %d, pid %d: received ", i + 1, getpid());
    for (j = 0; j < 13; j++){
        read(fdAssignCard[i][0], &playercard[j][i], 3);
        printf("%s ", &playercard[j][i]); // Print the value of child[j][3] instead of child[j]
    }
    printf("\n");

    close(fdAssignCard[i][0]);
}

// assign the card to the player
void assignCards(char *cards[52], int fdAssignCard[4][2]){
    int i;
    int s;

    for (i = 0; i < 4; i++){
        close(fdAssignCard[i][0]);
    }

    for (i = 0; i < 52; i++){
        write(fdAssignCard[i % 4][1], cards[i], 3);
    }

    for (i = 0; i < 4; i++){
        close(fdAssignCard[i][1]);
    }
}

// read the file and store the card to the array cards
void readFile(int i, char *cards[52], char *argv[])
{

    FILE *file = fopen(argv[1], "r");
    if (file == NULL){
        printf("Failed to open the file.\n");
        exit(1);
    }
    char card[3];
    while (fscanf(file, "%2s", card) == 1){
        cards[i] = malloc(3 * sizeof(char));
        strcpy(cards[i], card);
        i++;
    }
    fclose(file);
}

int main(int argc, char *argv[]){
    int i, j;
    char childcard[13][3];
    char *cards[52];

    int childNo;
    int processPid;
    char playedCardRecord[52];
    char currentRoundPlayedCard[3];

    int fd[2];
    int fdAssignCard[4][2];
    int fdParentToChild[4][2];
    int fdChildToParent[4][2];

    int playerOrder[4];

    readFile(i, cards, argv);

    pid_t pid;

    if (pipe(fd) < 0){
        printf("Pipe creation error\n");
        exit(1);
    }
    for (i = 0; i < 4; i++){
        if (pipe(fdAssignCard[i]) < 0){
            printf("Pipe creation error1\n");
            exit(1);
        }
        if (pipe(fdParentToChild[i]) < 0){
            printf("Pipe creation error2\n");
            exit(1);
        }
        if (pipe(fdChildToParent[i]) < 0){
            printf("Pipe creation error3\n");
            exit(1);
        }
    }

    //////////////////////// Child Process ////////////////////////
    //////////////////////// Child Process ////////////////////////
    for (i = 0; i < 4; i++){
        char childReceiveBuffer[80];
        char childSendBuffer[80];

        childNo = i + 1;
        pid = fork();
        if (pid < 0){
            fprintf(stderr, "Fork failed\n");
            return 1;
        }
        else if (pid == 0){
            int currentPid = getpid();
            close(fd[0]);
            write(fd[1], &currentPid, sizeof(currentPid));
            close(fd[1]);
            
            readAssignedCards(i, fdAssignCard, childcard);
            sortTheCardSuit(i, fdAssignCard, childcard);
            for (j = 0; j < 4; j++){
                close(fdAssignCard[j][0]);
            }
            ssize_t bytesLength;
            char playCard[3];

            // keep reading the parent to child pipe
            while ((bytesLength = read(fdParentToChild[i][0], childReceiveBuffer, sizeof(childReceiveBuffer))) > 0){

                // if the parent send endgame, then end
                if (strcmp(childReceiveBuffer, "endgame") == 0){
                    for (i = 0; i < 4; i++){
                        close(fdChildToParent[i][0]);
                        close(fdChildToParent[i][1]);
                        close(fdParentToChild[i][0]);
                        close(fdParentToChild[i][1]);
                    }
                    exit(0);
                }
                //clear playCard
                memset(playCard, 0, sizeof(playCard));
                // find the play card for the player
                findPlayCard(i, childcard, playCard, childReceiveBuffer, playedCardRecord);

                printf("Child %d pid %d: play %s \n", i + 1, getpid(), playCard);

                strcat(playedCardRecord, playCard);
                strcat(playedCardRecord, " ");
                // printf("playedCardRecord %s \n", playedCardRecord);
                strcpy(currentRoundPlayedCard, playCard);
                // printf("currend round sent card %s \n", currentRoundPlayedCard);
                // write the play card to the parent //////////////////
                write(fdChildToParent[i][1], currentRoundPlayedCard, sizeof(currentRoundPlayedCard));
                bytesLength = 0;

                memset(childReceiveBuffer, 0, sizeof(childReceiveBuffer));
            }

            return 0;
        }
    }

    //////////////////////// Parent Process ////////////////////////
    //////////////////////// Parent Process ////////////////////////

    if (pid > 0){

        int round = 1;
        int currentPlayer = 0, currentmark = 0;
        int winner = 0;
        int firstPlayer = 0;
        char sendBuffer[80];
        char receiveBuffer[80];
        int score[4] = {0};
        int j = 0;

        printf("Parent pid %d: child players are", getpid());
        for (i = 0; i < 4; i++){
            read(fd[0], &processPid, sizeof(processPid));
            printf(" %d", processPid);
        }
        printf("\n");

        assignCards(cards, fdAssignCard);

        for (i = 0; i < 4; i++){
            close(fdAssignCard[i][0]);
            close(fdAssignCard[i][1]);
        }

        sleep(1);

        for (i = 0; i < 13; i++){ ////////////////////round/////////////
            printf("Parent pid %d: round %d child %d to lead", getpid(), i + 1, firstPlayer + 1);
            printf("\n");

            for (j = 0; j < 4; j++){
                currentPlayer = (firstPlayer + j) % 4;
                playerOrder[j] = currentPlayer;

                if (j == 0){
                    strcpy(sendBuffer, "go");
                }
                write(fdParentToChild[currentPlayer][1], &sendBuffer, sizeof(sendBuffer));

                if (j == 0){
                    memset(&sendBuffer, 0, sizeof(sendBuffer));
                }
                sleep(1);

                read(fdChildToParent[currentPlayer][0], receiveBuffer, sizeof(receiveBuffer));

                printf("Parent pid %d: child %d plays %s\n", getpid(), currentPlayer+1, receiveBuffer);

                // add value receiveBuffer to sendBuffer
                strcat(sendBuffer, receiveBuffer);
                strcat(sendBuffer, " ");
            }
            // add sendBuffer to checkwin function
            winner = checkWin(sendBuffer, playerOrder);
            firstPlayer = winner;
            score[winner] = calScore(sendBuffer, score, winner);

            // sleep(1);
            memset(&sendBuffer, 0, sizeof(sendBuffer));
            memset(&receiveBuffer, 0, sizeof(receiveBuffer));
            memset(playerOrder, 0, sizeof(playerOrder));
        }
        for (i = 0; i < 4; i++){
            close(fdParentToChild[i][0]);
            close(fdChildToParent[i][0]);
            write(fdParentToChild[i][1], "endgame", sizeof("endgame"));
            close(fdParentToChild[i][1]);
            close(fdChildToParent[i][1]);
        }
        printf("Parent pid %d: game completed\n", getpid());
        printf("Parent pid %d: score = <%d %d %d %d>\n", getpid(), score[0], score[1], score[2], score[3]);
    }

    for (i = 0; i < 4; i++){
        wait(NULL);
    }
    return 0;
}
