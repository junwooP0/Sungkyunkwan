//¹ÚÁØ¿ì 2022316101 //


#include<stdio.h>
#define MAX_DEGREE 101

typedef struct {
	int degree;
	float coef[MAX_DEGREE];
}polynomial;


polynomial Attach(polynomial x, float Coef, int Exp) {
	int temp;
	if (Exp > x.degree) {
		temp = Exp - x.degree;
		x.degree = Exp;
		for (int i = Exp + 1;i > temp; i--)
			x.coef[i] = x.coef[i - temp];
		x.coef[1] = Coef;
	}
	else {
		x.coef[x.degree - Exp + 1] += Coef;
	}
	return x;
}

polynomial Remove(polynomial x, int Exp) {
	x.coef[x.degree - Exp + 1] = 0;
	if (Exp == x.degree) {
		int a = 1;
		while (x.coef[a] == 0) a++;
		for (int i = 0; i <= x.degree - a + 1; i++)
			x.coef[i] = x.coef[i + a];
	}
	return x;
}

int a[100];

int main() {
	float a = 4.5;
	printf("%0.5f", a);
	return 0;
}