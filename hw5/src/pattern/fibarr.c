int MAIN() {
    int a[20], i;
    a[0] = 1;
    a[1] = 1;
    for (i = 2; i < 20; i = i + 1)
	a[i] = a[i-1] + a[i-2];
    for (i = 0; i < 20; i = i + 1) {
	write(a[i]);
	write("\n");
    }
    return 0;
}
