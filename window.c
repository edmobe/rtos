#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "mainlogic.h"

/*=========================== MAIN ===========================*/
int main()
{
    /* Initialize alien array, report and random seed */
    initialize();
    srand(time(NULL));

    /* Add processes */
    append(2, 6);
    append(4, 9);

    printf("Created array is: ");
    printAlienArray();
    printf("\n");

    printMaze();

    for (int i = 0; i < 26; i++)
    {
        if (i == 9)
        {
            append(4, 9);
            append(1, 9);
        }
        edf(i);
        sleep(1);
    }

    // Print report
    for (int i = 0; i < report.iterations; i++)
    {
        if (report.log[i] == -1)
            printf("[%d - %d]: ?\n", i, i + 1);
        else
            printf("[%d - %d]: Process %d\n", i, i + 1, report.log[i]);
    }

    return 0;
}