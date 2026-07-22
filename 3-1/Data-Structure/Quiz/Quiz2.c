#include <stdio.h>
#define SWAP(x,y,t) ((t) = (x), (x) = (y), (y) = (t))

int main() {
	int x, y, z;
	int temp;
	scanf("%d%d%d", &x, &y, &z);
	if (x > y)
		SWAP(x, y, temp);
	if (x > z)
		SWAP(x, z, temp);
	if (y > z)
		SWAP(y, z, temp);
	printf("Ascending order is %d %d %d\n", x, y, z);

	return 0;
}