int compare(int a, int b) {
	int x = 9;
	int y = 9;
	int z = 9;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			for (int k = 0; k < 10; k++) {
				if (x < i) {
					if (y < j) {
						if (z < k) {
							return x;
						}
					}
				}
			}	
		}
	}
	return 0;
}	