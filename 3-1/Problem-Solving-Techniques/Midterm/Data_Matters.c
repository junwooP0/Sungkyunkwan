#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int n1;
int n2;

int binary(char **Arr, int first, int last, char *value)
{
    while (first <= last)
    {
        int mid = first + (last - first) / 2;
        int standard = strcmp(Arr[mid], value);

        if (standard == 0)
            return mid;
        else if (standard < 0)
            first = mid + 1;
        else
            last = mid - 1;
    }

    return -1;
}

int countCommonStrings(char **strArr1, char **strArr2)
{
    int count = 0;
    int start = 0;
    int found = 0;

    if (n1 >= 100000 && n2 > n1 * 10)
    {
        for (int i = 0; i < n1; i++)
        {
            start = binary(strArr2, 0, n2 - 1, strArr1[i]);
            if (start != -1)
                break;
        }

        int index1 = 0;
        int index2 = start;

        while (index1 < n1 && index2 < n2)
        {
            int cmp = strcmp(strArr1[index1], strArr2[index2]);
            if (cmp == 0)
            {
                count++;
                index1++;
                index2++;
            }
            else if (cmp < 0)
            {
                index1++;
            }
            else
            {
                index2++;
            }
        }
    }

    else if (n2 > n1 * 100)
    {

        for (int i = 0; i < n1; i++)
        {
            found = binary(strArr2, start, n2 - 1, strArr1[i]);
            if (found != -1){
                count++;
							  start = found + 1;
						}
        }
    }
    else
    {

        int index1 = 0;
        int index2 = 0;
        while (index1 < n1 && index2 < n2)
        {
            int cmp = strcmp(strArr1[index1], strArr2[index2]);
            if (cmp == 0)
            {
                count++;
                index1++;
                index2++;
            }
            else if (cmp < 0)
            {
                index1++;
            }
            else
            {
                index2++;
            }
        }
    }

    return count;
}


int main()
{
  scanf("%d", &n1);
  scanf("%d", &n2);
  char** strArr1 = (char**) malloc(sizeof(char*)*n1);
  char** strArr2 = (char**) malloc(sizeof(char*)*n2);
    
  for(int i=0; i<n1; i++)
    strArr1[i] = (char*) malloc(sizeof(char)*9);
  for(int i=0; i<n2; i++)
    strArr2[i] = (char*) malloc(sizeof(char)*9);

  for(int i=0; i<n1; i++)
    scanf("%s", *(strArr1 + i));
  for(int i=0; i<n2; i++)
    scanf("%s", *(strArr2 + i));
 
  int sum = 0;
  int test_num = 150;
  for(int i=0; i<test_num; i++)
    sum+=countCommonStrings(strArr1, strArr2);
  printf("%d\n", sum/test_num);

  free(strArr1);
  free(strArr2);

  return 0;
}