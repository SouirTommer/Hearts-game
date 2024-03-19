#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

int main()
{
    int	fd[2];
    int fd2[4][2];
    int i = 0;
    int j;
    char *cards[52];

    pid_t pid;
    char child1[40] = {0};
    char child2[40] = {0};
    char child3[40] = {0};
    char child4[40] = {0};
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


            // sleep(rand()%2);
            int currentPid = getpid();
            close(fd[0]);
            write(fd[1], &currentPid, sizeof(currentPid));
            close(fd[1]);


            

            if (childNo == 1){

                for(i=0; i<13; i++){
                    // read(fd2[0][0], "hi", 3);
                    // printf(" %c\n", child1[i]);
                }
                // printf("Child %d, pid %d: %s\n", childNo, getpid(), child1);
                // sortTheCardSuit(child1);

                // printGroupedCards(childNo, child1);
            }

            // else if (childNo == 2){
            //     printf("Child %d, pid %d: %s\n", childNo, getpid(), child2);
            //     // sortTheCardSuit(child2);

            //     // printGroupedCards(childNo, child2);
            // }
            // else if (childNo == 3){
            //     printf("Child %d, pid %d: %s\n", childNo, getpid(), child3);
            //     // sortTheCardSuit(child3);

            //     // printGroupedCards(childNo, child3);
            // }
            // else if (childNo == 4){
            //     printf("Child %d, pid %d: %s\n", childNo, getpid(), child4);
            //     // sortTheCardSuit(child4);

            //     // printGroupedCards(childNo, child4);
            // }

            return 0;
        }

    }
    printf("Parent pid %d: child players are", getpid());
    for (i = 0; i<4; i++){
        read(fd[0], &childNo, sizeof(childNo));
        printf(" %d", childNo);
    }
    printf("\n");

    int maxChild = 4;

    for (i = 0; i<52; i++){
        write(fd2[i % maxChild][1], cards[i], 3);
    }



    for (i = 0; i < 4; i++){
        wait(NULL);
    }
    return 0;
}
