#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_PROCESSES 5
#define DIMENSIONS 20
#define RM_MAX 0.6931471807

struct Alien
{
    int finished;
    int posX;
    int posY;
    int direction;
    int energy;
    int period;
    pthread_t threadId;
};

struct AlienArray
{
    struct Alien aliens[MAX_PROCESSES];
    int length;
};

void initialize(struct AlienArray *alienArray)
{
    alienArray->length = 0;
}

float getUtilization(struct AlienArray *alienArray)
{
    float utilization = 0;
    for (int i = 0; i < alienArray->length; i++)
    {
        if (!alienArray->aliens[i].finished)
            utilization += (float)alienArray->aliens[i].energy / (float)alienArray->aliens[i].period;
    }
    return utilization;
}

void *
print(void *arg)
{
    printf("Thread initialized, energy = %d\n", *((int *)arg));
}

int append(struct AlienArray *alienArray, int energy, int period)
{
    /* Determines if the new process can be scheduled */
    float utilization = getUtilization(alienArray);
    utilization += (float)energy / (float)period;

    if (utilization >= 1)
    {
        printf("Error: proccess is not planificable.\n");
        return 0;
    }

    if (alienArray->length + 1 > MAX_PROCESSES)
    {
        printf("Error: max amount of proccesses is %d.\n", MAX_PROCESSES);
        return 0;
    }

    printf("Utilization: %f\n", utilization);

    alienArray->aliens[alienArray->length].energy = energy;
    alienArray->aliens[alienArray->length].period = period;
    alienArray->aliens[alienArray->length].direction = 'r';
    alienArray->aliens[alienArray->length].finished = 0;
    alienArray->aliens[alienArray->length].posX = 0;
    alienArray->aliens[alienArray->length].posY = 7;
    pthread_create(&alienArray->aliens[alienArray->length].threadId, NULL, print, &alienArray->aliens[alienArray->length].energy);
    pthread_join(alienArray->aliens[alienArray->length].threadId, NULL);
    alienArray->length++;

    return 1;
}

// This function prints contents of the alien array
void printAlienArray(struct AlienArray *alienArray)
{
    float sum = 0;
    for (int i = 0; i < alienArray->length; i++)
    {
        if (!alienArray->aliens[i].finished)
        {
            printf("[%d, %d] ", alienArray->aliens[i].energy, alienArray->aliens[i].period);
            sum += (float)alienArray->aliens[i].energy / (float)alienArray->aliens[i].period;
        }
    }
    printf("\nUtilization: %f\n", sum);
}

void printMaze(int maze[DIMENSIONS][DIMENSIONS])
{
    for (int i = 0; i < DIMENSIONS; i++)
    {
        for (int j = 0; j < DIMENSIONS; j++)
        {
            if (maze[i][j])
            {
                printf("██");
            }
            else
            {
                printf("  ");
            }
        }
        printf("\n");
    }
}

int main()
{
    /* Start with the empty list */
    struct AlienArray alienArray;
    initialize(&alienArray);

    append(&alienArray, 1, 6);
    append(&alienArray, 2, 9);
    append(&alienArray, 17, 18);

    printf("Created Linked list is: ");
    printAlienArray(&alienArray);
    printf("\n");

    int maze[DIMENSIONS][DIMENSIONS] =
        {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
            {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
            {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
            {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
            {1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        };

    printMaze(maze);

    return 0;
}