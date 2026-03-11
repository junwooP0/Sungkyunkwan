#include<stdio.h>
#include<stdlib.h>
#include<time.h>

typedef struct node
{
    int data;
    struct node *next;
} Node;

typedef struct Linkedlist
{
    Node *head;
    Node *tail;
} Llist;

void insert(Llist *lst, int x){
    Node *new;
    new = (Node *)malloc(sizeof(Node));
    if(new == NULL){
        printf("Error");
        return;
    }
    new->data = x;
    new->next = NULL;

    if (lst->head == NULL || lst->head->next == NULL) {
        new->next = lst->head;
        lst->head = new;     
        lst->tail = (lst->head->next == NULL) ? new : lst->head->next;
    }
    else {
        Node *now = lst->head;
        for(; now->next->next != NULL; now = now->next);
        new->next = now->next;
        now->next = new;
        lst->tail = new;
    }
}

void delete(Llist *lst, int x) {
    Node *now = lst->head;
    Node *previous = NULL;

    for(int position = 1; now != NULL; position++){
        Node *next = now->next;

        if (position % 2 == 1 && now->data == x) {
            previous == NULL ? (lst->head = next) : (previous->next = next);
            free(now);
        } else {

            previous = now;
        }

        now = next;
    }

    lst->tail = previous;
}


void print(Llist *lst) {
    Node *now = lst->head;
    int total = 0;
    for(int i = 0; now != NULL; i++, now = now->next) {
        total++;
    }
    int half = (total + 1) / 2;
    now = lst->head;

    for (int i = 0; i < half; i++) {
        printf("%d ", now->data);
        now = now->next;
    }

    printf("\n");

    for (int i = half; i < total; i++) {
        printf("%d ", now->data);
        now = now->next;
    }

    printf("\n");
}

int main(){
    srand(time(NULL));
    int arr[100];
    Llist list;
    Llist *lst = &list;
    lst->head = NULL;
    lst->tail = NULL;
    for(int i = 0; i < 100; i++){
        arr[i] = rand() % 100;
    }
    for(int i = 0; i < 100; i++){
        insert(lst, arr[i]);
    }
    for(int i = 0 ; i < 3; i++){
        printf("Before deletion\n");
        print(lst);
        int random = rand() % 100;
        delete(lst, arr[random]);
        printf("After deletion\n");
        print(lst);
    }

    return 0;
}
