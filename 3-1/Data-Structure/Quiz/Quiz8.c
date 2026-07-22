#include <stdio.h>


// 2022316101 ╧заь©Л

for (int i = 0; i < numCols; i++) { startingPos[i] = 0; }

for (int i = 1; i <= numTerms; i++) {
    startingPos[a[i].col]++;
}

startingPos[0] = 1;
int last = startingPos[0];
int current

for (int i = 1; i < numCols; i++) {
    current = startingPos[i];
    startingPos[i] = startingPos[i - 1] + last;
    last = current;
}