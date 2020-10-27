#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

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
    int id, finished, posX, posY, direction, energy, period;
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

/*=========================== EDF SCHEDULING ===========================*/
void edf(int iteration)
{
    // Get the higher priority alien
    alienArray.higherPriorityDeadline = __INT_MAX__;
    alienArray.higherPriorityIndex = -1;
    for (int j = 0; j < alienArray.length; j++)
    {
        if (alienArray.remainingEnergies[j] > 0)
        {
            if (alienArray.higherPriorityDeadline > alienArray.nextDeadline[j] ||
                (alienArray.higherPriorityDeadline == alienArray.nextDeadline[j] &&
                 alienArray.remainingEnergies[alienArray.higherPriorityIndex] > alienArray.remainingEnergies[j]))
            {
                alienArray.higherPriorityDeadline = alienArray.nextDeadline[j];
                alienArray.higherPriorityIndex = j;
            }
        }
    }

    // Add to report
    report.log[iteration] = alienArray.higherPriorityIndex;
    report.iterations++;
    printf("From %d to %d: Process %d\n", iteration, iteration + 1, alienArray.higherPriorityIndex);

    // Spend energy
    pthread_mutex_lock(&mutex);
    currentThread = alienArray.higherPriorityIndex;
    pthread_mutex_unlock(&mutex);
    alienArray.remainingEnergies[alienArray.higherPriorityIndex]--;

    // Next deadline
    for (int j = 0; j < alienArray.length; j++)
    {
        if (alienArray.newPeriods[j] == (alienArray.aliens[j].period - 1))
        {
            alienArray.nextDeadline[j] = alienArray.aliens[j].period;
            alienArray.remainingEnergies[j] = alienArray.aliens[j].energy;
            alienArray.newPeriods[j] = 0;
        }
        else
        {
            if (alienArray.nextDeadline[j] > 0)
            {
                alienArray.nextDeadline[j]--;
            }
            else
            {
                if (alienArray.remainingEnergies[j] > 0)
                {
                    printf("Error: Process %d cannot be scheduled\n", j);
                    return;
                }
            }
            alienArray.newPeriods[j]++;
        }
    }
}

/*=========================== INITIALIZING METHODS ===========================*/
// Initializes an alien array and the final report.
void initialize()
{
    alienArray.length = 0;
    report.iterations = 0;
}

/*=========================== ALIEN METHODS ===========================*/
void move(int id)
{
    printf("Moving alien %d\n", id);
}

int validMove(int currentDirection, int destinationX, int destinationY)
{
    if (MAZE[destinationY][destinationX])
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

    for (size_t i = 0; i < alienArray.length; i++)
    {
        // Collision
        if (alienArray.aliens[i].direction == currentDirection &&
            alienArray.aliens[i].posX == destinationX &&
            alienArray.aliens[i].posY == destinationY)
        {
            printf("Invalid move: must wait for the alien.\n");
            return 0;
        }
    }

    return 1;
}

void *print(void *arg)
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
int append(int energy, int period)
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
    alienArray.aliens[alienArray.length].direction = 'r';
    alienArray.aliens[alienArray.length].finished = 0;
    alienArray.aliens[alienArray.length].posX = 0;
    alienArray.aliens[alienArray.length].posY = 7;
    pthread_create(&alienArray.aliens[alienArray.length].threadId, NULL, print, &alienArray.aliens[alienArray.length]);
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
    /* Initialize alien array and report */
    initialize(&alienArray, &report);

    /* Add processes */
    append(2, 6);
    append(4, 9);

    printf("Created array is: ");
    printAlienArray();
    printf("\n");

    printMaze();

    alienArray.aliens[0].posX = 2;
    alienArray.aliens[1].posX = 1;

    printf("Valid move? %d\n",
           validMove(alienArray.aliens[1].direction, 2, alienArray.aliens[1].posY));

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
            printf("From %d to %d: ?\n", i, i + 1);
        else
            printf("From %d to %d: Process %d\n", i, i + 1, report.log[i]);
    }

    return 0;
}