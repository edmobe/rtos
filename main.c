#include <stdio.h>

#define MAX_PROCCESSES 10

struct Alien
{
    int energy;
    int period;
};

struct AlienList
{
    struct Alien aliens[MAX_PROCCESSES];
    int length;
};

int initialize(struct AlienList *alienList)
{
    alienList->length = 0;
}

int addAlien(int energy, int period, struct AlienList *alienList)
{
    // Can be appended
    if (alienList->length < MAX_PROCCESSES)
    {
        struct Alien newAlien;
        newAlien.energy = energy;
        newAlien.period = period;
        alienList->aliens[alienList->length] = newAlien;
        alienList->length++;
        return 1;
    }
    return 0;
}

int main()
{
    // Initialize alienList
    struct AlienList alienList;
    initialize(&alienList);
    addAlien(10, 10, &alienList);
    addAlien(11, 11, &alienList);
    addAlien(12, 12, &alienList);
    addAlien(13, 13, &alienList);
    for (int i = 0; i < alienList.length; i++)
    {
        printf("%d. Energy: %d. Period: %d.\n", i, alienList.aliens[i].period, alienList.aliens[i].energy);
    }

    return 0;
}