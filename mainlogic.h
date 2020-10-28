#include <pthread.h>

/*=========================== CONSTANTS ===========================*/
#define MAX_PROCESSES 5
#define DIMENSIONS 20
#define MAX_ITERATIONS 1000
// #define RM_MAX 0.6931471807

static int MAZE[DIMENSIONS][DIMENSIONS];

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
static int currentThread;
static struct AlienArray alienArray;
static struct Report report;
pthread_mutex_t mutex;

/*=========================== EDF SCHEDULING ===========================*/
void edf(int iteration);

/*=========================== INITIALIZING METHODS ===========================*/
// Initializes an alien array and the final report.
void initialize();

/*=========================== ALIEN METHODS ===========================*/
int validMove(int currentDirection, int destinationX, int destinationY);

int moveRight(int id);

int moveLeft(int id);

int moveUp(int id);

int moveDown(int id);

int tryDirection(int id, char direction);

void shuffle(int *array, size_t n);

int move(int id);

void *initializeThread(void *arg);

/*=========================== ALIEN ARRAY OPERATIONS ===========================*/
// Calculates the utilization of the processes (aliens)
float getUtilization();

// If a new process (alien) can be managed, creates it and appends it to the alien array.
int append(int energy, int period, int iteration);

/*=========================== DISPLAY ===========================*/
// This function prints contents of the alien array
void printAlienArray();

// This function prints the maze
void printMaze();