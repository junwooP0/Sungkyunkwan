#include <stdio.h>

typedef unsigned char* pointer;

void show_bytes(pointer start, size_t len) {
    size_t i;
    for (i = 0; i < len; i++)
        printf("%p\t0x%x\n", (void*)(start + i), start[i]); // 책과 달리 (void*) 가 추가됨
    printf("\n");
}

int main() {
    unsigned int x = 15213;
    show_bytes((pointer)&x, sizeof(x));
    return 0;
}