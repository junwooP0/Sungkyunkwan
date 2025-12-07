#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LENGTH 100000

int findSavedStrength(char* food);

int main() {
	char *food = (char *)malloc(MAX_LENGTH * sizeof(char));
	
	if (food == NULL) {
		printf("Failed to allocate memory.\n");
		return 1;
	}
	
	printf("Enter Food: ");
	scanf("%s", food);
	
	printf("Saved Strength is: %d\n", findSavedStrength(food));
	
	free(food);
	
	return 0;
}



int findSavedStrength(char* food){
    	
	  int savedStrength = 0;
    int total = strlen(food);

    for (int len = total; len > 0; len--)
    {
        for (int start = 0; start <= total - len; start++)
        {
            int alpha[26] = {0};
            int cnt = 0;
            for (int i = start; i < start + len; i++)
            {
                alpha[food[i] - 'A']++;
            }

            for (int j = 0; j < 26; j++)
            {
                if (alpha[j] % 2 != 0)
                {
                    cnt++;
                }
            }

            if (cnt == 0 || cnt == 1)
            {
                savedStrength = len * 100 / 2;
                return savedStrength;
            }
        }
    }
}


