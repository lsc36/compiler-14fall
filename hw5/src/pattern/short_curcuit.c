int MAIN(){
    int a = 0;
    int tmp;
    if (0 || 1 || 0 || 0) {
        write("yes\n");
    }
    if (1 && 1 && 0 && 1) {
        write("no\n");
    }
    tmp = (a != 0 && write("no\n"));
    tmp = (a == 0 || write("no\n"));
    
	return 0;
}

