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
struct AlienArray alienArray;
struct Report report;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int finished = 0;

/*=========================== GENERAL SCHEDULING METHODS ===========================*/
void addSchedulingIterationToReport(int iteration);

void updateHigherPriorityProcess();

/*=========================== EDF SCHEDULING ===========================*/
void setEdfHigherPriorityProcess();

void setEdfNextProcess();

void edf(int iteration);

/*=========================== RM SCHEDULING ===========================*/
void setRmHigherPriorityProcess();

void setRmNextProcess();

void rm(int iteration);

/*=========================== INITIALIZING METHODS ===========================*/
void initialize();

/*=========================== FINISHING METHODS ===========================*/
void exitLogic();

/*=========================== ALIEN METHODS ===========================*/
int validMove(int currentDirection, int destinationX, int destinationY);

int moveRight(int id);

int moveLeft(int id);

int moveUp(int id);

int moveDown(int id);

int tryDirection(int id, char direction);

void shuffle(int *array, size_t n);

int allFinished();

int move(int id);

void *initializeThread(void *arg);

/*=========================== ALIEN ARRAY OPERATIONS ===========================*/
float getUtilization();

int append(int energy, int period, int iteration);

/*=========================== DISPLAY ===========================*/
void printAlienArray();

void printMaze();