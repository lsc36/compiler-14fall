int a[10];
int MAIN() {
    int i, j;
    int tmp;
    a[0] = 32;
    a[1] = 39;
    a[2] = 12;
    a[3] = 83;
    a[4] = 74;
    a[5] = 58;
    a[6] = 73;
    a[7] = 30;
    a[8] = 38;
    a[9] = 83;
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
