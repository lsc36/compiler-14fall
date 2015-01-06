int k;


void try(int p1, int p2, int p3, int p4) {
    if (k < 10) {
        write(k);
        write("\n");
        k = k + 1;
        try(1,2,3,4);
    }
}

int MAIN(int pa, int pb) {
    k = 0;
    try(1,2,3,4);
    return 0;
}
