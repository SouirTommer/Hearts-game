#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

//input suit such as 'C', then return the counts array
int findSuitCount(int *counts, char suit){
    if (suit == 'C')
    {
        return counts[0];
    }
    else if (suit == 'H')
    {
        return counts[1];
    }
    else if (suit == 'D')
    {
        return counts[2];
    }
    else if (suit == 'S')
    {
        return counts[3];
    }
    return 0;
}
// input the cards array, then will help you to calculate the suit counts
void calculateSuitCounts(char* cards, int* suitCounts) {
    int n = strlen(cards);
    int i;
    for (i = 0; i < 5; i++) {
        suitCounts[i] = 0;
    }
    // Calculate suit counts
    for (i = 0; i < n; i += 3) {
        char suit = cards[i];
        if (suit == 'C') {
            suitCounts[0]++;
        } else if (suit == 'H') {
            suitCounts[1]++;
        } else if (suit == 'D') {
            suitCounts[2]++;
        } else if (suit == 'S') {
            suitCounts[3]++;
        } else if (suit == 'J' || suit == 'j') {
            suitCounts[4]++;
        }
    }
}
//use bubble sort to sort the cards, first sort the card based on suit, then sort the card based on the suit counts 
void sortTheCardSuit(char* cards) {
    int n = strlen(cards);
    int i, j;
	char temp;
	char tempNumber;

    int suitCounts[5] = {0};
    calculateSuitCounts(cards, suitCounts);

    char rank[] = "AKQJT98765432";
    
    for (i = 0; i < n - 3; i += 3) {
        for (j = 0; j < n - 3 - i; j += 3) {
            // Compare the suits of adjacent cards
            if (cards[j] > cards[j + 3]) {
                // Swap the suits
                temp = cards[j];
                cards[j] = cards[j + 3];
                cards[j + 3] = temp;

				tempNumber = cards[j + 1];
                cards[j + 1] = cards[j + 4];
                cards[j + 4] = tempNumber;
            }
            // Compare the suits of adjacent cards
            if (cards[j] == cards[j + 3]){
                // Compare the ranks of adjacent cards
                if (strchr(rank, cards[j + 1]) > strchr(rank, cards[j + 4])){
                    // Swap the ranks
                    temp = cards[j];
                    cards[j] = cards[j + 3];
                    cards[j + 3] = temp;

                    tempNumber = cards[j + 1];
                    cards[j + 1] = cards[j + 4];
                    cards[j + 4] = tempNumber;
                }
            }
            // if suit 'C' have 2, then suit 'H' have 4, then 2 < 4, then swap the cards
            if (findSuitCount(suitCounts, cards[j]) < findSuitCount(suitCounts, cards[j + 3])){

                temp = cards[j];
                cards[j] = cards[j + 3];
                cards[j + 3] = temp;

                tempNumber = cards[j + 1];
                cards[j + 1] = cards[j + 4];
                cards[j + 4] = tempNumber;
            }
        }
    }
}

// Print the cards in the format of <C2 C3 C4> <H5 H6 H7> <D>
void printGroupedCards(int childNo, char* cards) {
    int n = strlen(cards);
    int i;
	int suitCount = 0;
    printf("Child %d, pid %d: <", childNo, getpid());
    for (i = 0; i < n; i += 3) {
        // card suit is different from the current card, the print > <
        if (i > 0 && cards[i] != cards[i - 3]) {
			suitCount++;
            printf("> <");
        } 
		// if next card is different, don't need the space
		if(cards[i + 3] != cards[i]) {
        	printf("%c%c", cards[i], cards[i + 1]);
		} else{
			printf("%c%c ", cards[i], cards[i + 1]);
		}
    }
    printf(">");

    // if only 3 suit, print <> that means no cards in this suit
	for(; suitCount < 3; suitCount++){
		printf("%s", " <>");
	}
	printf("\n");

}
// loop array t calculate the points
int calculateHighCardPoints(char* cards) {
    int n = strlen(cards);
    int i;
    int points = 0;
    
    for (i = 1; i < n; i += 3) {
        char rank = cards[i];
        
        if (rank == 'A')
            points += 4;
        else if (rank == 'K')
            points += 3;
        else if (rank == 'Q')
            points += 2;
        else if (rank == 'J')
            points += 1;
    }
    
    return points;
}

int calculateAdditionalPoints(char* cards) {
    int n = strlen(cards);
    int suitCounts[5] = {0};
    int points = 0;

    char rank;
    char suit;
    int i;
    // Count the number of cards in each suit
    calculateSuitCounts(cards, suitCounts);
    
    // Calculate points based on suit length
    for (i = 0; i < 4; i++) {
        int suitLength = suitCounts[i];

         // Check for void, singleton, and doubleton
        if (suitLength == 0){
            points += 3;  // Void
        }
        else if (suitLength == 1){
            points += 2;  // Singleton
        }
        else if (suitLength == 2){
            points += 1;  // Doubleton
        }
        else if (suitLength == 5){
            points += 1;  // 5 cards in a suit
        }
        else if (suitLength == 6){
            points += 2;  // 6 cards in a suit
        }
        else if (suitLength >= 7){
            points += 3; // 7 or more cards in a suit
        }
    }
    int j;
    for (j = 0; j < n; j += 3) {
        suit = cards[j];
        rank = cards[j + 1];

        if ((suit == 'C' && suitCounts[0] == 1) ||  // Singleton honor in Spades
            (suit == 'H' && suitCounts[1] == 1) ||  // Singleton honor in Hearts
            (suit == 'D' && suitCounts[2] == 1) ||  // Singleton honor in Diamonds
            (suit == 'S' && suitCounts[3] == 1)) {  // Singleton honor in Clubs
            if (rank == 'A' || rank == 'K' || rank == 'Q' || rank == 'J'){
                points -= 1;
            }
        }
    }
    return points;
}

// Check if a card is repeated in the deck
bool isRepeat(char* cards, char suit, char rank){
    int n = strlen(cards);
    int i;
    for (i = 0; i < n; i += 3){
        if (cards[i] == suit && cards[i+1] == rank){
            return true;
        }
    }
    return false;
}

// Move the joker card
void moveTheJoker(char* cards){
    int n = strlen(cards);

    char suits[] = {'C', 'H', 'D', 'S'};
    char rank[] = {'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'};

    int jIndex = 99;
    int JIndex = 99;

    int highCardScore;
    int addScore;
    int maxscore = 0;
    int i;
    int s;
    int r;
    // Find the index of the joker card
    for (i = 0; i < n; i += 3){
        if (cards[i] == 'j'){
            jIndex = i;
        }
        if (cards[i] == 'J'){
            JIndex = i;
        }
    }


    // if found, that mean the index have value
    if (JIndex != 99){
        highCardScore = calculateHighCardPoints(cards);
        addScore = calculateAdditionalPoints(cards);
        // use inner loop to find highest score, s means suit, r means rank
        for (s = 0; s < 4; s++){
            for (r = 0; r < 13; r ++){
                    
                if (!isRepeat(cards, suits[s], rank[r])){

                    if((highCardScore + addScore) > maxscore){

                        cards[JIndex] = suits[s];
                        cards[JIndex+1] = rank[r];

                        highCardScore = calculateHighCardPoints(cards);
                        addScore = calculateAdditionalPoints(cards);
                        maxscore = highCardScore + addScore;
                        printf("[ ! ] Pid: %d, Replace JJ card to %c%c\n", getpid(),cards[JIndex], cards[JIndex+1]);
                    }
                }
            }
        }
    }

    maxscore = 0;
    // because player may have two joker, so need to check too
    if (jIndex != 99){
        highCardScore = calculateHighCardPoints(cards);
        addScore = calculateAdditionalPoints(cards);
        
        for (s = 0; s < 4; s++){
            for (r = 0; r < 13; r ++){
                
                if (!isRepeat(cards, suits[s], rank[r])){

                    if((highCardScore + addScore) > maxscore){

                        cards[jIndex] = suits[s];
                        cards[jIndex+1] = rank[r];

                        highCardScore = calculateHighCardPoints(cards);
                        addScore = calculateAdditionalPoints(cards);
                        maxscore = highCardScore + addScore;
                        printf("[ ! ] Pid: %d, Replace jj card to %c%c\n", getpid() ,cards[jIndex], cards[jIndex+1]);
                    }
                }   
            } 
        }
    }
}

// Create child processes using fork
void createFork(char *argv[]){
    pid_t pid;
    char child1[40] = {0};
    char child2[40] = {0};
    char child3[40] = {0};
    char child4[40] = {0};
    int points;
    int additionalPoints;
    int i;
    int suitCounts[5] = {0};

    for (i = 0; i < 4; i++){
        int childNo = i + 1;

        pid = fork();
        if (pid < 0){
            fprintf(stderr, "Fork failed\n");
            return;
        }
        else if (pid == 0){
            sleep(rand()%2);
            // Child process
            for (i = 1; argv[i] != '\0'; i++){
                if (childNo == 1 && (i - 1) % 4 == 0){
                    strcat(child1, argv[i]);
                    strcat(child1, " ");
                }
                else if (childNo == 2 && (i - 2) % 4 == 0){
                    strcat(child2, argv[i]);
                    strcat(child2, " ");
                }
                else if (childNo == 3 && (i - 3) % 4 == 0){
                    strcat(child3, argv[i]);
                    strcat(child3, " ");
                }
                else if (childNo == 4 && (i - 4) % 4 == 0){
                    strcat(child4, argv[i]);
                    strcat(child4, " ");
                }
            }
            if (childNo == 1){
                printf("Child %d, pid %d: %s\n", childNo, getpid(), child1);
                sortTheCardSuit(child1);

                calculateSuitCounts(child1, suitCounts);

                if (suitCounts[4] > 0){
                    printf("[ ! ] Detected a joker in Child 1, now moving the joker...\n");
                    moveTheJoker(child1);
                    sortTheCardSuit(child1);
                }

                printGroupedCards(childNo, child1);
                points = calculateHighCardPoints(child1);
                additionalPoints = calculateAdditionalPoints(child1);
            }
            else if (childNo == 2){
                printf("Child %d, pid %d: %s\n", childNo, getpid(), child2);
                sortTheCardSuit(child2);

                calculateSuitCounts(child2, suitCounts);

                if (suitCounts[4] > 0){
                    printf("[ ! ] Detected a joker in Child 2, now moving the joker...\n");
                    moveTheJoker(child2);
                    sortTheCardSuit(child2);
                }

                printGroupedCards(childNo, child2);
                points = calculateHighCardPoints(child2);
                additionalPoints = calculateAdditionalPoints(child2);
            }
            else if (childNo == 3){
                printf("Child %d, pid %d: %s\n", childNo, getpid(), child3);
                sortTheCardSuit(child3);

                calculateSuitCounts(child3, suitCounts);

                if (suitCounts[4] > 0){
                    printf("[ ! ] Detected a joker in Child 3, now moving the joker...\n");
                    moveTheJoker(child3);
                    sortTheCardSuit(child3);
                }

                printGroupedCards(childNo, child3);
                points = calculateHighCardPoints(child3);
                additionalPoints = calculateAdditionalPoints(child3);
            }
            else if (childNo == 4){
                printf("Child %d, pid %d: %s\n", childNo, getpid(), child4);
                sortTheCardSuit(child4);

                calculateSuitCounts(child4, suitCounts);

                if (suitCounts[4] > 0){
                    printf("[ ! ] Detected a joker in Child 4, now replacing the joker...\n");
                    moveTheJoker(child4);
                    sortTheCardSuit(child4);
                }

                printGroupedCards(childNo, child4);
                points = calculateHighCardPoints(child4);
                additionalPoints = calculateAdditionalPoints(child4);
            }
                printf("Child %d, pid %d: %d points, %d adjusted points\n", childNo, getpid(), points, additionalPoints + points);
                exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
    createFork(argv);
    int i;
    for (i = 0; i < 4; i++){
        wait(NULL);
    }
    return 0;
}
