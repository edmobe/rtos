#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/*=========================== CONSTANTS ===========================*/
#define MAX_PROCESSES 5
#define DIMENSIONS 20
#define MAX_ITERATIONS 1000
// #define RM_MAX 0.6931471807

static int MAZE[DIMENSIONS][DIMENSIONS] =
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

/*=========================== STRUCTURES ===========================*/
struct Alien
{
    int id, finished, posX, posY, direction, energy, period, appendedIteration;
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

/*=========================== GLOBAL VARIABLES ===========================*/
static int currentThread = -1;
static struct AlienArray alienArray;
static struct Report report;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int finished = 0;

/*=========================== GENERAL SCHEDULING METHODS ===========================*/
void addSchedulingIterationToReport(int iteration)
{
    report.log[iteration] = alienArray.higherPriorityIndex;
    report.iterations++;
    printf("[%d - %d]: Process %d.\n", iteration, iteration + 1, alienArray.higherPriorityIndex);
}

void updateHigherPriorityProcess()
{
    pthread_mutex_lock(&mutex);
    currentThread = alienArray.higherPriorityIndex;
    pthread_mutex_unlock(&mutex);
    alienArray.remainingEnergies[alienArray.higherPriorityIndex]--;
}

/*=========================== EDF SCHEDULING ===========================*/
void setEdfHigherPriorityProcess()
{
    alienArray.higherPriorityDeadline = __INT_MAX__;
    alienArray.higherPriorityIndex = -1;
    for (int i = 0; i < alienArray.length; i++)
    {
        if (!alienArray.aliens[i].finished && alienArray.remainingEnergies[i] > 0)
        {
            if (alienArray.higherPriorityDeadline > alienArray.nextDeadline[i] ||
                (alienArray.higherPriorityDeadline == alienArray.nextDeadline[i] &&
                 alienArray.remainingEnergies[alienArray.higherPriorityIndex] > alienArray.remainingEnergies[i]))
            {
                alienArray.higherPriorityDeadline = alienArray.nextDeadline[i];
                alienArray.higherPriorityIndex = i;
            }
        }
    }
}

void setEdfNextProcess()
{
    for (int i = 0; i < alienArray.length; i++)
    {
        if (!alienArray.aliens[i].finished)
        {
            if (alienArray.newPeriods[i] == (alienArray.aliens[i].period - 1))
            {
                alienArray.nextDeadline[i] = alienArray.aliens[i].period;
                alienArray.remainingEnergies[i] = alienArray.aliens[i].energy;
                alienArray.newPeriods[i] = 0;
            }
            else
            {
                if (alienArray.nextDeadline[i] > 0)
                {
                    alienArray.nextDeadline[i]--;
                }
                else
                {
                    if (alienArray.remainingEnergies[i] > 0)
                    {
                        printf("Error: Process %d cannot be scheduled.\n", i);
                        return;
                    }
                }
                alienArray.newPeriods[i]++;
            }
        }
    }
}

void edf(int iteration)
{
    setEdfHigherPriorityProcess();

    addSchedulingIterationToReport(iteration);

    updateHigherPriorityProcess();

    setEdfNextProcess();
}

/*=========================== RM SCHEDULING ===========================*/
void setRmHigherPriorityProcess()
{
    alienArray.higherPriorityDeadline = __INT_MAX__;
    alienArray.higherPriorityIndex = -1;
    for (int i = 0; i < alienArray.length; i++)
    {
        if (!alienArray.aliens[i].finished && alienArray.remainingEnergies[i] > 0)
        {
            if (alienArray.higherPriorityDeadline > alienArray.nextDeadline[i])
            {
                alienArray.higherPriorityDeadline = alienArray.nextDeadline[i];
                alienArray.higherPriorityIndex = i;
            }
        }
    }
}

void setRmNextProcess()
{
    for (int i = 0; i < alienArray.length; i++)
    {
        if (!alienArray.aliens[i].finished)
        {
            alienArray.nextDeadline[i]--;
            if (alienArray.nextDeadline[i] == 0)
            {
                alienArray.nextDeadline[i] = alienArray.aliens[i].period;
                alienArray.remainingEnergies[i] = alienArray.aliens[i].energy;
            }
        }
    }
}

void rm(int iteration)
{
    setRmHigherPriorityProcess();

    addSchedulingIterationToReport(iteration);

    updateHigherPriorityProcess();

    setRmNextProcess();
}

/*=========================== INITIALIZING METHODS ===========================*/
// Initializes an alien array and the final report.
void initialize()
{
    alienArray.length = 0;
    report.iterations = 0;
}

/*=========================== FINISHING METHODS ===========================*/
void exitLogic()
{
    finished = 1;
    printf("Here goes the exit logic.\n");
}

/*=========================== ALIEN METHODS ===========================*/
int validMove(int currentDirection, int destinationX, int destinationY)
{
    if (MAZE[destinationY][destinationX])
    {
        // There is a wall
        return 0;
    }

    if (destinationX < 0 || destinationY < 0)
    {
        // Out of bounds
        return 0;
    }

    for (int i = 0; i < alienArray.length; i++)
    {
        // Collision
        if (alienArray.aliens[i].direction == currentDirection &&
            alienArray.aliens[i].posX == destinationX &&
            alienArray.aliens[i].posY == destinationY)
        {
            return 0;
        }
    }

    return 1;
}

int moveRight(int id)
{
    if (validMove('r', alienArray.aliens[id].posX + 1, alienArray.aliens[id].posY))
    {
        alienArray.aliens[id].posX++;
        alienArray.aliens[id].direction = 'r';
        printf("End position: (%d, %d). Direction: %c.\n", alienArray.aliens[id].posX,
               alienArray.aliens[id].posY, alienArray.aliens[id].direction);
        return 1;
    }
    return 0;
}

int moveLeft(int id)
{
    if (validMove('l', alienArray.aliens[id].posX - 1, alienArray.aliens[id].posY))
    {
        alienArray.aliens[id].posX--;
        alienArray.aliens[id].direction = 'l';
        printf("End position: (%d, %d). Direction: %c.\n", alienArray.aliens[id].posX,
               alienArray.aliens[id].posY, alienArray.aliens[id].direction);
        return 1;
    }
    return 0;
}

int moveUp(int id)
{
    if (validMove('u', alienArray.aliens[id].posX, alienArray.aliens[id].posY - 1))
    {
        alienArray.aliens[id].posY--;
        alienArray.aliens[id].direction = 'u';
        printf("End position: (%d, %d). Direction: %c.\n", alienArray.aliens[id].posX,
               alienArray.aliens[id].posY, alienArray.aliens[id].direction);
        return 1;
    }
    return 0;
}

int moveDown(int id)
{
    if (validMove('d', alienArray.aliens[id].posX, alienArray.aliens[id].posY + 1))
    {
        alienArray.aliens[id].posY++;
        alienArray.aliens[id].direction = 'd';
        printf("End position: (%d, %d). Direction: %c.\n", alienArray.aliens[id].posX,
               alienArray.aliens[id].posY, alienArray.aliens[id].direction);
        return 1;
    }
    return 0;
}

int tryDirection(int id, char direction)
{
    switch (direction)
    {
    case 0:
        if (moveRight(id))
        {
            return 1;
        }
        break;
    case 1:
        if (moveLeft(id))
        {
            return 1;
        }
        break;
    case 2:
        if (moveUp(id))
        {
            return 1;
        }
        break;
    case 3:
        if (moveDown(id))
        {
            return 1;
        }
        break;
    default:
        printf("Unknown direction!\n");
        return 0;
    }

    return 0;
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int allFinished()
{
    for (int i = 0; i < alienArray.length; i++)
    {
        if (!alienArray.aliens[i].finished)
        {
            return 0;
        }
    }
    return 1;
}

int move(int id)
{
    // If alien has finished
    if (alienArray.aliens[id].posX == 19 && alienArray.aliens[id].posY == 1)
    {
        printf("Alien %d finished successfully!\n", id);
        alienArray.aliens[id].finished = 1;
        alienArray.aliens[id].posX = -1;
        alienArray.aliens[id].posY = -1;
        if (allFinished())
        {
            exitLogic();
        }

        return 1;
    }

    // Try 4 directions
    printf("Moving alien %d from (%d, %d). ", id, alienArray.aliens[id].posX, alienArray.aliens[id].posY);
    int directions[3];
    switch (alienArray.aliens[id].direction)
    {
    case 'r':
        directions[0] = 1;
        directions[1] = 2;
        directions[2] = 3;
        if (moveRight(id))
        {
            return 1;
        }
        break;
    case 'l':
        directions[0] = 0;
        directions[1] = 2;
        directions[2] = 3;
        if (moveLeft(id))
        {
            return 1;
        }
        break;
    case 'u':
        directions[0] = 0;
        directions[1] = 1;
        directions[2] = 3;
        if (moveUp(id))
        {
            return 1;
        }
        break;
    case 'd':
        directions[0] = 0;
        directions[1] = 1;
        directions[2] = 2;
        if (moveDown(id))
        {
            return 1;
        }
        break;
    default:
        printf("Unknown direction!\n");
        return 0;
    }

    shuffle(&directions[0], (int)(sizeof(directions) / sizeof(directions[0])));
    for (int i = 0; i < 3; i++)
    {
        if (tryDirection(id, directions[i]))
        {
            return 1;
        }
    }

    printf("Cannot move alien %d to any direction!\n", id);
    return 0;
}

void *initializeThread(void *arg)
{
    struct Alien *alien = arg;
    printf("Thread %d initialized.\n", alien->id);
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (currentThread == alien->id)
        {
            currentThread = -1;
            move(alien->id);
        }
        pthread_mutex_unlock(&mutex);
    }
}

/*=========================== ALIEN ARRAY OPERATIONS ===========================*/
// Calculates the utilization of the processes (aliens)
float getUtilization()
{
    float utilization = 0;
    for (int i = 0; i < alienArray.length; i++)
    {
        if (!alienArray.aliens[i].finished)
            utilization += (float)alienArray.aliens[i].energy / (float)alienArray.aliens[i].period;
    }
    return utilization;
}

// If a new process (alien) can be managed, creates it and appends it to the alien array.
int append(int energy, int period, int iteration)
{
    /* Determines if the new process can be scheduled */
    float utilization = getUtilization(alienArray);
    utilization += (float)energy / (float)period;

    if (utilization >= 1)
    {
        printf("Error: proccess is not planificable.\n");
        return 0;
    }

    if (alienArray.length + 1 > MAX_PROCESSES)
    {
        printf("Error: max amount of proccesses is %d.\n", MAX_PROCESSES);
        return 0;
    }

    printf("Utilization: %f\n", utilization);

    alienArray.aliens[alienArray.length].id = alienArray.length;
    alienArray.aliens[alienArray.length].energy = energy;
    alienArray.aliens[alienArray.length].period = period;
    alienArray.aliens[alienArray.length].appendedIteration = iteration;
    alienArray.aliens[alienArray.length].direction = 'r';
    alienArray.aliens[alienArray.length].finished = 0;
    alienArray.aliens[alienArray.length].posX = 0;
    alienArray.aliens[alienArray.length].posY = 7;
    pthread_create(&alienArray.aliens[alienArray.length].threadId, NULL, initializeThread, &alienArray.aliens[alienArray.length]);
    // pthread_join(alienArray.aliens[alienArray.length].threadId, NULL);
    alienArray.length++;

    // Update periods and remaining energies
    for (int i = 0; i < alienArray.length; i++)
    {
        alienArray.nextDeadline[i] = alienArray.aliens[i].period;
        alienArray.remainingEnergies[i] = alienArray.aliens[i].energy;
        alienArray.newPeriods[i] = 0;
    }

    return 1;
}

/*=========================== DISPLAY ===========================*/
// This function prints contents of the alien array
void printAlienArray()
{
    float sum = 0;
    for (int i = 0; i < alienArray.length; i++)
    {
        if (!alienArray.aliens[i].finished)
        {
            printf("[%d, %d] ", alienArray.aliens[i].energy, alienArray.aliens[i].period);
            sum += (float)alienArray.aliens[i].energy / (float)alienArray.aliens[i].period;
        }
    }
    printf("\nUtilization: %f\n", sum);
}

// This function prints the maze
void printMaze()
{
    for (int i = 0; i < DIMENSIONS; i++)
    {
        for (int j = 0; j < DIMENSIONS; j++)
        {
            if (MAZE[i][j])
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
    /* Initialize alien array, report, and random seed */
    initialize();
    srand(time(NULL));

    /* Add processes */
    append(1, 6, 0);
    append(2, 9, 0);
    append(6, 18, 0);

    /* Modify positions to test exit */
    /*
    alienArray.aliens[0].posX = 17;
    alienArray.aliens[0].posY = 1;
    alienArray.aliens[1].posX = 16;
    alienArray.aliens[1].posY = 1;
    */

    printf("Created array is: ");
    printAlienArray();
    printf("\n");

    printMaze();

    int iterationCounter = 0;
    while (!finished)
    {
        rm(iterationCounter);
        sleep(1);
        iterationCounter++;
    }

    printf("================== REPORT ==================\n");

    // Print processes
    for (int i = 0; i < alienArray.length; i++)
    {
        printf("Process %d initialized in cycle %d\n",
               alienArray.aliens[i].id, alienArray.aliens[i].appendedIteration);
               
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