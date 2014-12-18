int factorial(int i) {
	if (i == 1) {
		return 1;
	}
	else {
		return i * factorial(i - 1);
	}
}

int conversion(int a) {
	float b = a;
	return b;
}

int main() {
	int a, b;
	float c, d;
	factorial(4);
	factorial(4.5);
	factorial(4, 4);
	factorial(a);
	factorial(c);
	factorial();
	c = factorial(a);
	b = conversion(b);
}
