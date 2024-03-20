#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

void sortTheCardSuit(int i, int fd2[4][2], char playercard[13][3]) {
    int j, k, h;
	char temp;
	char tempNumber;

    int suitCounts[5] = {0};

    // printf("test-- %c  \n", playercard[1][0]);
    
    // calculateSuitCounts(cards, suitCounts);

    char rank[] = "AKQJT98765432";
    
    //bubble sort

    // for (j = 0; j < 13; j++){
    //     for (k = 0; k < 13 - j - 1; k++){
    //         if (playercard[k][i] > playercard[k+1][i]){
    //             temp = playercard[k][i];
    //             playercard[k][i] = playercard[k+1][i];
    //             playercard[k+1][i] = temp;
    //         }
    //     }
    // }

    

    

    // printf("Child %d, pid %d: arranged ", i+1, getpid());
    // //print the array
    // for (h = 0; h < 13; h++){
    //     printf("%c ", playercard[h][i]);
    // }
    // printf("\n");

}

void readAssignedCards(int i, int fd2[4][2], char playercard[13][3]){
    int j;
    
        printf("Child %d, pid %d: received ", i+1, getpid());
        for(j=0; j<13; j++){
            read(fd2[i][0], &playercard[j][i], 3);
            printf("%s ", &playercard[j][i]); // Print the value of child[j][3] instead of child[j]
        }
        printf("\n");

        if(i==0){
        printf("1test-- %s%s  \n", &playercard[0][0], &playercard[1][0]);
        printf("2test-- %s%s  \n", &playercard[2][1], &playercard[3][0]);
        printf("3test-- %s%s  \n", &playercard[4][0], &playercard[5][0]);
        }
}

void assignCards(char *cards[52], int fd2[4][2]){
    int i;
    int s;

    for (i = 0; i < 4; i++){
        close(fd2[i][0]);
    }

    for (i = 0; i < 52; i++){
        write(fd2[i%4][1], cards[i], 3);
    }

    for (i = 0; i < 4; i++){
        close(fd2[i][1]);
    }

}


int main()
{
    int	fd[2];
    int fd2[4][2];
    int i = 0;
    int j;
    char childcard[13][3];
    char *cards[52];


    pid_t pid;
    int suitCounts[4] = {0};


    FILE *file = fopen("card1.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }
    char card[3];
    while (fscanf(file, "%2s", card) == 1) {
        cards[i] = malloc(3 * sizeof(char));
        strcpy(cards[i], card);
        i++;
    }
    fclose(file);

    if (pipe(fd) < 0) {
        printf("Pipe creation error\n");
        exit(1);
    }
    for(i = 0; i < 4; i++){
        if (pipe(fd2[i]) < 0) {
            printf("Pipe creation error\n");
            exit(1);
        }
    }

    // Create child processes using fork
    int childNo;
    for (i = 0; i < 4; i++){
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
           
            close(fd2[i][1]);

            readAssignedCards(i, fd2, childcard);
            sortTheCardSuit(i, fd2, childcard);
        for (j = 0; j < 4; j++){
            close(fd2[j][0]);
        }
            

            return 0;
        }
    }

    printf("Parent pid %d: child players are", getpid());
    for (i = 0; i<4; i++){
        read(fd[0], &childNo, sizeof(childNo));
        printf(" %d", childNo);
    }
    printf("\n");

    assignCards(cards, fd2);

    for (i = 0; i < 4; i++){
        wait(NULL);
    }
    return 0;
}
