#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_PROCESSES 5
#define RM_MAX 0.6931471807

struct Alien
{
    int energy;
    int period;
    struct Alien *next;
    pthread_t threadId;
};

void *print(void *arg)
{
    printf("Thread initialized, energy = %d\n", *((int *)arg));
}

/* Given a reference (pointer to pointer) to the head 
   of a list and an int, appends a new node at the end 

    AlgorithmId:
        0: RM
        1: EDF
*/
int append(struct Alien **head_ref, int energy, int period)
{
    /* 1. allocate node */
    struct Alien *newAlien = (struct Alien *)malloc(sizeof(struct Alien));

    struct Alien *last = *head_ref;                    /* used in step 5*/
    int len = 0;                                       /* used in step 5*/
    float utilization = (float)energy / (float)period; /* Determines if the new process can be scheduled */

    /* 2. put in the data  */
    newAlien->energy = energy;
    newAlien->period = period;

    /* 3. This new node is going to be the last node, so make next  
          of it as NULL */
    newAlien->next = NULL;

    /* 4. If the Linked List is empty, then make the new node as head */
    if (*head_ref == NULL)
    {
        if (utilization >= 1)
        {
            printf("Error: proccess is not planificable.\n");
            return 0;
        }
        pthread_create(&newAlien->threadId, NULL, print, &newAlien->energy);
        pthread_join(newAlien->threadId, NULL);
        *head_ref = newAlien;
        return 1;
    }

    utilization += (float)last->energy / (float)last->period;
    len++;

    /* 5. Else traverse till the last node */
    while (last->next != NULL)
    {
        last = last->next;
        utilization += (float)last->energy / (float)last->period;
        len++;
    }

    if (utilization >= 1)
    {
        printf("Error: proccess is not planificable.\n");
        return 0;
    }

    if (len + 1 > MAX_PROCESSES)
    {
        printf("Error: max amount of proccesses is %d.\n", MAX_PROCESSES);
        return 0;
    }

    /* 6. Start thread */
    pthread_create(&newAlien->threadId, NULL, print, &newAlien->energy);
    pthread_join(newAlien->threadId, NULL);

    /* 7. Change the next of last node */
    last->next = newAlien;

    return 1;
}

// This function prints contents of linked list starting from head
void printAlienList(struct Alien *alien)
{
    float sum = 0;
    while (alien != NULL)
    {
        printf("[%d, %d] ", alien->energy, alien->period);
        sum += (float)alien->energy / (float)alien->period;
        alien = alien->next;
    }
    printf("\nUtilization: %f\n", sum);
}

int main()
{
    /* Start with the empty list */
    struct Alien *head = NULL;

    /*
    // Test
    append(&head, 2, 9, 1);
    append(&head, 3, 20, 1);
    append(&head, 5, 30, 1);
    append(&head, 4, 20, 1);
    append(&head, 10, 25, 1);
    append(&head, 1, 50, 1);
    append(&head, 2, 15, 1);
    append(&head, 1, 18, 1);
    append(&head, 2, 15, 1);
    */

    append(&head, 1, 6);
    append(&head, 2, 9);
    append(&head, 6, 18);

    printf("Created Linked list is: ");
    printAlienList(head);
    printf("\n");

    return 0;
}