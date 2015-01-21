int MAIN() {
    float a[10];
    int i, j;
    float tmp;
    a[0] = 32.2;
    a[1] = 39.1;
    a[2] = 12.3;
    a[3] = 83.1;
    a[4] = 74.9;
    a[5] = 58.5;
    a[6] = 73.8;
    a[7] = 30.0;
    a[8] = 38.4;
    a[9] = 83.2;
    i = 0;
    j = 0;
    while (i < 10) {
        j = 0;
        while (j < (10 - 1)) {
            if (a[j] < a[j + 1]) {
                tmp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = tmp;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    i = 0;
    while (i < 10) {
        write(a[i]);
        write("\n");
        i = i + 1;
    }
}
