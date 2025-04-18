#include "testheader.h"
#include "codegen.h"
#define MAX_NUMBERS 100
#define MAX_SHOES 200
#define NUMBER 5

int function() {
	int x = 2;
	int y = 20;
	int z = MAX_NUMBERS + y;
	int w = z + MAX_SHOES;

	return w;
}

int function2() {
	int x = 10;
	int y = 20;
	int z = x + y;
	return z;
}

int function3() {
	int x = 10;
}

int main() {
	int x = NUMBER;
	return 0;
}