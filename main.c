#include <stdio.h>

#define MAX_PROCCESSES 5

struct Alien
{
    int energy;
    int period;
};

int isPlanificable(struct Alien *alien, struct AlienList *alienList)
{
    float sum = 0;
    for (int i = 0; i < alienList->length; i++)
    {
        sum += alienList->aliens[i].energy / alienList->aliens[i].period;
    }
    if (sum < 1)
    {
        return 1;
    }
    return 0;
}

int addAlien(int energy, int period, struct AlienList *alienList)
{
    // Can be appended
    if (alienList->length < MAX_PROCCESSES)
    {
        struct Alien newAlien;
        newAlien.energy = energy;
        newAlien.period = period;
        if (isPlanificable(&newAlien, alienList))
        {
            alienList->aliens[alienList->length] = newAlien;
            alienList->length++;
        }
        else
        {
            printf("Error: proccess is not planificable.\n");
            return 0;
        }
        return 1;
    }
    printf("Error: max amount of proccesses is %d.\n", MAX_PROCCESSES);
    return 0;
}

int main()
{
    // Initialize alienList
    struct AlienList alienList;
    initializeAlienList(&alienList);
    addAlien(3, 10, &alienList);
    addAlien(1, 11, &alienList);
    addAlien(2, 12, &alienList);
    addAlien(1, 13, &alienList);
    addAlien(2, 13, &alienList);
    addAlien(3, 13, &alienList);
    for (int i = 0; i < alienList.length; i++)
    {
        printf("%d. Energy: %d. Period: %d.\n", i, alienList.aliens[i].period, alienList.aliens[i].energy);
    }

    return 0;
}