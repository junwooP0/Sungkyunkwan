#include <stdio.h>
#include <stdlib.h>


int main() {
	int i = 1;
	while(i<=5){
		if(i == 2) continue;
		printf("%d\n", i);
		i++;
	}
	return 0;
}