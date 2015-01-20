int MAIN(){
    int a[20];
    int sum = 0;
    int i = 5;
    while (i <= 15) {
        write(i);
        write("\n");
        a[i] = i;
        i = i + 1;
    }
    i = 5;
    while (i <= 15) {
        sum = sum + a[i];
        write(a[i]);
        write("\n");
        i = i + 1;
    }
    write(sum);
}
