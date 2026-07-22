
 //박준우 2022316101

listPointer invertedCopyList(listPointer ptr){
    listPointer copy = NULL;
    listPointer temp = NULL;
    while(ptr != NULL){
        temp = (listPointer)malloc(sizeof(listNode));
        temp->link = copy;
        copy = temp;
        ptr = ptr->link;
    }
    return copy;
}