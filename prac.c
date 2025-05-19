#define MAX_NUMBERS 100
#define MAX_SHOES 200
#define NUMBER 5

int add(int a, int b) {
	return a + b;
}

int function() {
	int x = 2;
	int y = 3;
	y++;

	int arr[3] = {1, 6, 8};
	
	for (int i = 0; i < 3; i++) {
		if (arr[i] < 10) {
			arr[i] = 5;
		} else if (arr[i] > 10) {
			arr[i] = 2;
		} else {
			arr[i] = 8;
		}
	}

	int z = 3;
	int sum = add(z, 4);

	return 0;
}

int main() {
	int x = 20;

	return 0;
}