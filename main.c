#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*=========================== CONSTANTS ===========================*/
#define MAX_PROCESSES 5
#define DIMENSIONS 20
#define MAX_ITERATIONS 1000
// #define RM_MAX 0.6931471807

/*=========================== STRUCTURES ===========================*/
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
    int nextDeadline[MAX_PROCESSES], remainingEnergies[MAX_PROCESSES], newPeriods[MAX_PROCESSES];
    int length, higherPriorityDeadline, higherPriorityIndex;
};

struct Report
{
    int iterations;
    int log[MAX_ITERATIONS]; // [from, to, pid]
};

/*=========================== EDF SCHEDULING ===========================*/
void edf(struct AlienArray *alienArray, struct Report *report, int iteration)
{
    // Get the higher priority alien
    alienArray->higherPriorityDeadline = __INT_MAX__;
    alienArray->higherPriorityIndex = -1;
    for (int j = 0; j < alienArray->length; j++)
    {
        if (alienArray->remainingEnergies[j] > 0)
        {
            if (alienArray->higherPriorityDeadline > alienArray->nextDeadline[j] ||
                (alienArray->higherPriorityDeadline == alienArray->nextDeadline[j] &&
                 alienArray->remainingEnergies[alienArray->higherPriorityIndex] > alienArray->remainingEnergies[j]))
            {
                alienArray->higherPriorityDeadline = alienArray->nextDeadline[j];
                alienArray->higherPriorityIndex = j;
            }
        }
    }

    // Add to report
    report->log[iteration] = alienArray->higherPriorityIndex;
    report->iterations++;

    // Spend energy
    alienArray->remainingEnergies[alienArray->higherPriorityIndex]--;

    // Next deadline
    for (int j = 0; j < alienArray->length; j++)
    {
        if (alienArray->newPeriods[j] == (alienArray->aliens[j].period - 1))
        {
            alienArray->nextDeadline[j] = alienArray->aliens[j].period;
            alienArray->remainingEnergies[j] = alienArray->aliens[j].energy;
            alienArray->newPeriods[j] = 0;
        }
        else
        {
            if (alienArray->nextDeadline[j] > 0)
            {
                alienArray->nextDeadline[j]--;
            }
            else
            {
                if (alienArray->remainingEnergies[j] > 0)
                {
                    printf("Error: Process %d cannot be scheduled\n", j);
                    return;
                }
            }
            alienArray->newPeriods[j]++;
        }
    }
}

/*=========================== INITIALIZING METHODS ===========================*/
// Initializes an alien array.
void initialize(struct AlienArray *alienArray, struct Report *report)
{
    alienArray->length = 0;
    report->iterations = 0;
}

void *print(void *arg)
{
    printf("Thread initialized, energy = %d\n", *((int *)arg));
}

/*=========================== CALCULATIONS ===========================*/
// Calculates the utilization of the processes (aliens)
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

int validMove(struct AlienArray *alienArray, int maze[DIMENSIONS][DIMENSIONS],
              int currentDirection, int destinationX, int destinationY)
{
    if (maze[destinationY][destinationX])
    {
        // There is a wall
        printf("Invalid move: there is a wall.\n");
        return 0;
    }

    if (destinationX < 0 || destinationY < 0)
    {
        // Out of bounds
        printf("Invalid move: out of bounds.\n");
        return 0;
    }

    for (size_t i = 0; i < alienArray->length; i++)
    {
        // Collision
        if (alienArray->aliens[i].direction == currentDirection &&
            alienArray->aliens[i].posX == destinationX &&
            alienArray->aliens[i].posY == destinationY)
        {
            printf("Invalid move: must wait for the alien.\n");
            return 0;
        }
    }

    return 1;
}

/*=========================== ARRAY OPERATIONS ===========================*/
// If a new process (alien) can be managed, creates it and appends it to the alien array.
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

    // Update periods and remaining energies
    for (int i = 0; i < alienArray->length; i++)
    {
        alienArray->nextDeadline[i] = alienArray->aliens[i].period;
        alienArray->remainingEnergies[i] = alienArray->aliens[i].energy;
        alienArray->newPeriods[i] = 0;
    }

    return 1;
}

/*=========================== DISPLAY ===========================*/
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

// This function prints the maze
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

/*=========================== MAIN ===========================*/
int main()
{
    /* Initialize alien array and report */
    struct AlienArray alienArray;
    struct Report report;
    initialize(&alienArray, &report);

    /* Add processes */
    append(&alienArray, 2, 6);
    append(&alienArray, 4, 9);

    printf("Created array is: ");
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

    alienArray.aliens[0].posX = 2;
    alienArray.aliens[1].posX = 1;

    printf("Valid move? %d\n",
           validMove(&alienArray, maze, alienArray.aliens[1].direction, 2, alienArray.aliens[1].posY));

    for (size_t i = 0; i < 26; i++)
    {
        if (i == 9)
        {
            append(&alienArray, 4, 9);
            append(&alienArray, 1, 9);
        }
        edf(&alienArray, &report, i);
    }

    // Print report
    for (int i = 0; i < report.iterations; i++)
    {
        if (report.log[i] == -1)
            printf("From %d to %d: ?\n", i, i + 1);
        else
            printf("From %d to %d: Process %d\n", i, i + 1, report.log[i]);
    }

    return 0;
}