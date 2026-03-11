#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SWAP(x, y, temp) ((temp) = (x), (x) = (y), (y) = (temp))

void createRandom(int *arr)
{
    int i = 0;
    while (i < 100)
    {
        int unique = 1;
        int random = rand() % 1000;

        for (int j = 0; j < i; j++)
        {
            if (arr[j] == random)
            {
                unique = 0;
                break;
            }
        }
        if (unique)
        {
            arr[i] = random;
            i++;
        }
    }
}

void ascended(int *arr)
{
    srand(time(NULL));
    int i = 0;

    for (int j = 1; j <= 179; j += 2)
    {
        arr[i] = j;
        i++;
    }

    while (i < 100)
    {

        int unique = 1;
        int random = rand() % 200;

        for (int j = 0; j < i; j++)
        {
            if (arr[j] == random)
            {
                unique = 0;
                break;
            }
        }

        if (unique)
        {
            arr[i] = random;
            i++;
        }
    }
}

void descended(int *arr)
{
    srand(time(NULL));
    int i = 0;
    for (int j = 179; j >= 1; j -= 2)
    {
        arr[i] = j;
        i++;
    }
    while (i < 100)
    {
        int unique = 1;
        int random = rand() % 200;
        for (int j = 0; j < i; j++)
        {
            if (arr[j] == random)
            {
                unique = 0;
                break;
            }
        }
        if (unique)
        {
            arr[i] = random;
            i++;
        }
    }
}

int partition_a(int *arr, int low, int high, int *count)
{
    int pivot = arr[high];
    int i = low;
    int temp = 0;

    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] <= pivot)
        {
            SWAP(arr[i], arr[j], temp);
            i++;
            (*count)++;
        }
    }
    SWAP(arr[i], arr[high], temp);
    return i;
}

void quickSort_a(int *arr, int low, int high, int *count)
{
    if (low < high)
    {
        int position = partition_a(arr, low, high, count);

        quickSort_a(arr, low, position - 1, count);

        quickSort_a(arr, position + 1, high, count);
    }
}

int partition_b(int *arr, int low, int high, int *count)
{
    int last = high;
    int mid = high - 1;
    int first = high - 2;
    int temp = 0;
    int pivotIdx;

    if (((arr[first] < arr[mid]) && (arr[first] > arr[last])) || ((arr[first] > arr[mid]) && (arr[first] < arr[last])))
        pivotIdx = first;
    else if (((arr[mid] < arr[first]) && (arr[mid] > arr[last])) || ((arr[mid] > arr[first]) && (arr[mid] < arr[last])))
        pivotIdx = mid;
    else
        pivotIdx = last;

    SWAP(arr[pivotIdx], arr[low], temp);

    int pivot = arr[low];
    int i = low;
    for (int j = low + 1; j <= high; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            SWAP(arr[i], arr[j], temp);
            (*count)++;
        }
    }
    SWAP(arr[low], arr[i], temp);
    return i;
}
void quickSort_b(int *arr, int low, int high, int *count)
{
    if (low < high)
    {
        int position = partition_b(arr, low, high, count);

        quickSort_b(arr, low, position - 1, count);

        quickSort_b(arr, position + 1, high, count);
    }
}

int partition_c(int *arr, int low, int high, int *count)
{
    srand(time(NULL));
    int range = high - low + 1;
    int randomIdx = low + rand() % range;
    int temp = 0;
    SWAP(arr[randomIdx], arr[low], temp);

    int pivot = arr[low];
    int i = low;

    for (int j = low + 1; j <= high; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            SWAP(arr[i], arr[j], temp);
            (*count)++;
        }
    }

    SWAP(arr[low], arr[i], temp);
    return i;
}
void quickSort_c(int *arr, int low, int high, int *count)
{
    if (low < high)
    {
        int position = partition_c(arr, low, high, count);

        quickSort_c(arr, low, position - 1, count);

        quickSort_c(arr, position + 1, high, count);
    }
}

int partition_d(int *arr, int low, int high, int *count)
{
    int range = high - low + 1;
    int randomIdx = low + rand() % range;
    int temp = 0;
    SWAP(arr[randomIdx], arr[low], temp);

    int pivot = arr[low];
    int i = low;

    for (int j = low + 1; j <= high; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;
            SWAP(arr[i], arr[j], temp);
            (*count)++;
        }
    }

    SWAP(arr[low], arr[i], temp);

    int total = high - low + 1;
    int splitpoint = low + total / 5;

    if (i < splitpoint)
    {
        return -1;
    }

    return i;
}

void quickSort_d(int *arr, int low, int high, int *count)
{
    if (low < high)
    {
        int position = partition_d(arr, low, high, count);

        if (position != -1)
        {
            quickSort_d(arr, low, position - 1, count);

            quickSort_d(arr, position + 1, high, count);
        }
        else
        {
            quickSort_d(arr, low, high, count);
        }
    }
}

void print(int *arr)
{
    for (int i = 0; i < 100; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main()
{
    srand(time(NULL));
    int A[100];
    int count = 0;

    createRandom(A);
    printf("Case a-1) Array filled with random number:\n");
    print(A);
    quickSort_a(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);
    count = 0;
    ascended(A);
    printf("\nCase a-2) Almost already sorted array:\n");
    print(A);
    quickSort_a(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);
    count = 0;
    descended(A);
    printf("\nCase a-3) Almost reversely sorted array:\n");
    print(A);
    quickSort_a(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);

    count = 0;
    createRandom(A);
    printf("\nCase b-1) Array filled with random number:\n");
    print(A);
    quickSort_b(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);
    count = 0;
    ascended(A);
    printf("\nCase b-2) Almost already sorted array:\n");
    print(A);
    quickSort_b(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);
    count = 0;
    descended(A);
    printf("\nCase b-3) Almost reversely sorted array:\n");
    print(A);
    quickSort_b(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);

    count = 0;
    createRandom(A);
    printf("\nCase c-1) Array filled with random number:\n");
    print(A);
    quickSort_c(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);
    count = 0;
    ascended(A);
    printf("\nCase c-2) Almost already sorted array:\n");
    print(A);
    quickSort_c(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);
    count = 0;
    descended(A);
    printf("\nCase c-3) Almost reversely sorted array:\n");
    print(A);
    quickSort_c(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);

    count = 0;
    createRandom(A);
    printf("\nCase d-1) Array filled with random number:\n");
    print(A);
    quickSort_d(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);
    count = 0;
    ascended(A);
    printf("\nCase d-2) Almost already sorted array:\n");
    print(A);
    quickSort_d(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);
    count = 0;
    descended(A);
    printf("\nCase d-3) Almost reversely sorted array:\n");
    print(A);
    quickSort_d(A, 0, 99, &count);
    printf("Sorted:\n");
    print(A);
    printf("Number of comparisons: %d\n", count);

    return 0;
}
