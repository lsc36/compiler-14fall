int fib(int n) {
    if (n <= 1) {
        return 1;
    }
    return fib(n-1) + fib(n-2);
}
float ffib(float n) {
    if (n <= 1.0) {
        return 1.0;
    }
    return ffib(n-1.0) + ffib(n-2.0);
}

int MAIN() {
    int i = 0;
    float j = 0.0;
    while (j < 10.0) {
        write(ffib(j));
        write("\n");
        j = j + 1.0;
    }
    while (i < 10) {
        write(fib(i));
        write("\n");
        i = i + 1;
    }
}
