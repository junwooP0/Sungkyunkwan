#define MAX_QUEUE_SIZE 5



//박준우 2022316101

element queue[MAX_QUEUE_SIZE];
int front = 0, rear = 0;
 //utilize all the space
void addq(element item){
    int count = 0;
    if(count == MAX_QUEUE_SIZE - 1){
        queueFull(rear);
    }
    rear = (rear + 1) % MAX_QUEUE_SIZE;
    queue[rear] = item;
    count++;
}

element deleteq()
{
    int count = 0;
    if(count == 0){
        queueEmpty();
    }
    front = (front + 1) % MAX_QUEUE_SIZE;
    count--;
    return queue[front];
}