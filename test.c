
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

int main()
{

        int playerOrder[] = {0,1,2,3};

        // print player order

        for (int i = 0; i < 4; i++)
        {
                printf("Player %d\n", playerOrder[i]);
        }

}