
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

int main()
{

        int playerOrder[] = {0,1,2,30,4};

        int maxIndex = 0;
        for (int i = 1; i < 5; i++) {
            if (playerOrder[i] > playerOrder[maxIndex]) {
                maxIndex = i;
            }
        }
        printf("The index of the biggest number is: %d\n", maxIndex);

}