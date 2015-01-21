int MAIN() {
    int a[2][3];
    int i = 0, j = 0;
    int sum = 0;
    while (i < 2) {
        j = 0;
        while (j < 3) {
            a[i][j] = (i + 1) * (j + 1);
            j = j + 1;
        }
        i = i + 1;
    }
    i = 0;
    j = 0;
    while (i < 2) {
        j = 0;
        while (j < 3) {
            sum = sum + a[i][j];
            write(a[i][j]);
            write("\n");
            j = j + 1;
        }
        i = i + 1;
    }
    write(sum);
}
