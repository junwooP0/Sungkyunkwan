#include <stdio.h>
#include <stdlib.h>
/* You cannot add other libraries */
void print_mountain_info(const int **mountain, int mountain_size);
const int **alloc_and_init_mountain(int mountain_size);

#define SWAP(x, y, t) ((t) = (x), (x) = (y), (y) = (t))

void print_mountain_info(const int **mountain, int mountain_size)
{
    for (int i = 0; i < mountain_size; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {
            printf("%d", mountain[i][j]);
            if (j == i)
                printf("\n");
            else
                printf(" ");
        }
    }
}

const int **alloc_and_init_mountain(int mountain_size)
{
    int **mountain_org;
    mountain_org = (int **)malloc(sizeof(int *) * mountain_size);
    for (int i = 0; i < mountain_size; i++)
    {
        mountain_org[i] = (int *)malloc(sizeof(int) * (i + 1));
    }
    srand(mountain_size);
    for (int i = 0; i < mountain_size; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {
            mountain_org[i][j] = (int)(rand() % 9999 + 1);
        }
    }
    return (const int **)mountain_org;
}

int find_max(const int **mountain, int mountain_size)
{
    int *next = (int *)malloc(sizeof(int) * mountain_size);
    int *current = (int *)malloc(sizeof(int) * mountain_size);
    int max = 0;
    int left, right;
    int *temp;

    current[0] = mountain[0][0];

    for (int i = 1; i < mountain_size; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            if (j == 0)
                left = 0;
            else
                left = current[j - 1];

            if (j > i - 1)
                right = 0;
            else
                right = current[j];

            if (left > right)
                next[j] = left + mountain[i][j];
            else
                next[j] = right + mountain[i][j];
        }

        SWAP(current, next, temp);
    }

    for (int i = 0; i < mountain_size; i++)
    {
        if (current[i] > max)
            max = current[i];
    }

    free(next);
    free(current);
    return max;
}
int main()
{
    int mountain_size = 0;

    scanf("%d", &mountain_size);

    const int **mountain = alloc_and_init_mountain(mountain_size);

    int result = find_max(mountain, mountain_size);
    printf("%d", result);
    for (int i = 0; i < mountain_size; i++)
    {
        free((int *)mountain[i]);
    }
    free((int **)mountain);

    return 0;
}