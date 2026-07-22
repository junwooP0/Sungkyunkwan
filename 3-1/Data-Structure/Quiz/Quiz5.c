#include<stdio.h>

// ¹ÚÁØ¿ì 2022316101 //

typedef struct {
	int lenght;
	int width;
}rectangle;

typedef struct {
	int s1;
	int s2;
	int s3;
}triangle;

typedef struct {
	int r;
}circle;

typedef struct {
	rectangle rec;
	triangle tri;
	circle cir;
}figure;



int main() {
	figure h;
	h.rec.lenght = 5;
	printf("%d", h.rec.lenght);
	return 0;
}