#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "golomb.hpp"
int main() {
	srand(time(NULL));
	int new_m=0, new_b=0, new_c=0;
	GolombsCode(m, b, c);
	printf("\n\nThe algorithm using optimum parameter m.\n\n");
	new_m = getM(p);
	new_b = getB(new_m);
	new_c = getC(new_m, new_b);
	GolombsCode(new_m, new_b, new_c);
	return 0;
}
