//20011367 ¹ÚÁØ¿ì

void stringRemove(char* s, int i, int j) {
    int l = strlen(s); 
    if (i + j > l) {
        fprintf(stderr, "position is out of bounds\n");
        exit(1);
    }
    else {
        for (int a = i; a < l - j; a++) {
            s[a] = s[a + j];
        }
        s[l - j] = '\0';
    }
}