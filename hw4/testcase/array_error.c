int func(int a[][10], int b) {
	return 1;
}

int main() {
	int a[10][10];
	int b = a[3][2];
	b = a[2.1][4];
	b = a[2.1 * 4 - 2][4]
	b = a[3];
	b = a[1][1][1];

	func(a, b); // correct
	func(a[1], b); // correct
	func(a, a[1][1]); // correct
	func(a[1][1], a[1][1]); // incorrect
}
