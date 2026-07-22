#define MAX_STACK_SIZE 100
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//박준우, 2022316101

//stack1, stack2 를 만든다

int main(){
    element stack1, stack2;
    for(int i = 0 ; i < 10; i++){
        int a = ran()%100;
        stack1.push(a);
    }
    for(int i = 0; i <10; i ++){
        stack2.push(stack1.pop());
    }
    for(int i = 0; i < 10; i++){
        printf("%d", stack2.pop());
    }

}