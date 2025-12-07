#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define size 100000

void static_array()
{
    static int arr[size] = {0};
}

void stack_array()
{
    int arr[size] = {0};
}

void heap_array()
{
    int *arr = (int *)malloc(sizeof(int) * size);
    free(arr);
}

int main()
{
    double time;
    clock_t start, end;

    start = clock();
    for (int i = 0; i < 1000000; i++)
    {
        static_array();
    }
    end = clock();
    time = ((double)(end - start) / CLOCKS_PER_SEC);
    printf("static array: %.6f sec\n", time);

    start = clock();
    for (int i = 0; i < 1000000; i++)
    {
        stack_array();
    }
    end = clock();
    time = ((double)(end - start) / CLOCKS_PER_SEC);
    printf("array on the stack: %.6f sec\n", time);

    start = clock();
    for (int i = 0; i < 1000000; i++)
    {
        heap_array();
    }
    end = clock();
    time = ((double)(end - start) / CLOCKS_PER_SEC);
    printf("array from the heap: %.6f sec\n", time);

    return 0;
}