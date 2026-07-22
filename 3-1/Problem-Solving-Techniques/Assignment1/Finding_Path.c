#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_WORD_LENGTH 5
#define MAX_WORDS 500

int findYulMyungPaths(const char *yul, const char *myung, char **wordList, int wordListSize);

typedef struct Node
{
    int idx;
    struct Node *next;
} Node;

Node *cNode(int idx)
{
    Node *n = malloc(sizeof(Node));
    n->idx = idx;
    n->next = NULL;
    return n;
} // 노드 생성

typedef struct
{
    Node *front;
    Node *rear;
} Queue;

typedef struct Graph
{
    int vertices;
    Node **connections;
} Graph; // 그래프

Queue *cQueue()
{
    Queue *q = malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    return q;
} // 큐 생성

bool Filled(Queue *q)
{
    return q->front != NULL;
}

void enqueue(Queue *q, int idx)
{
    Node *nNode = cNode(idx);
    if (!q->front)
    {
        q->front = nNode;
        q->rear = nNode;
        return;
    }
    q->rear->next = nNode;
    q->rear = nNode;
}

int dequeue(Queue *q)
{
    if (!q->front)
        return -1;
    Node *remove = q->front;
    int idx = remove->idx;
    q->front = remove->next;
    if (!q->front)
        q->rear = NULL;
    free(remove);
    return idx;
}

Graph *cGraph(int vertices)
{
    Graph *g = malloc(sizeof(Graph));
    g->vertices = vertices;
    g->connections = malloc(vertices * sizeof(Node *));
    for (int i = 0; i < vertices; i++)
    {
        g->connections[i] = NULL;
    }
    return g;
} // 그래프 생성

void addEdge(Graph *graph, int from, int to)
{
    Node *nNode = cNode(to);
    nNode->next = graph->connections[from];
    graph->connections[from] = nNode;

    nNode = cNode(from);
    nNode->next = graph->connections[to];
    graph->connections[to] = nNode;
} // 간선 추가

bool Convertable(const char *from, const char *to)
{
    int len = strlen(from);
    int diff = 0;
    for (int i = 0; i < len; i++)
    {
        if (from[i] != to[i])
        {
            diff++;
            if (diff > 1)
            {
                break;
            }
        }
    }

    if (diff == 1)
    {
        return true;
    }

    for (int i = 0; i < len; i++)
    {
        if (from[i] != to[len - i - 1])
        {
            return false;
        }
    } // 미러링 확인

    return true;
} // 변환 가능지 확인

void clear_input_buffer();

int main()
{
    char yul[MAX_WORD_LENGTH + 1], myung[MAX_WORD_LENGTH + 1];
    char line[(MAX_WORDS + 1) * (MAX_WORD_LENGTH + 1) + 1];
    char *wordList[MAX_WORDS];
    int wordListSize = 0;

    printf("Enter yul: ");
    scanf("%s", yul);
    clear_input_buffer(); // Clear the newline left by scanf

    printf("Enter myung: ");
    scanf("%s", myung);
    clear_input_buffer(); // Clear the newline left by scanf

    printf("Enter words separated by spaces: ");
    if (fgets(line, sizeof(line), stdin))
    {
        // Remove trailing newline, if any
        line[strcspn(line, "\n")] = 0;

        // Tokenize the input string
        char *token = strtok(line, " ");
        while (token != NULL && wordListSize < MAX_WORDS)
        {
            wordList[wordListSize] = malloc(strlen(token) + 1);
            if (wordList[wordListSize] == NULL)
            {
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }
            strcpy(wordList[wordListSize], token);
            wordListSize++;
            token = strtok(NULL, " ");
        }
    }

    int result = findYulMyungPaths(yul, myung, wordList, wordListSize);
    printf("Total paths with minimum word transformation: %d\n", result);

    // Free allocated memory
    for (int i = 0; i < wordListSize; i++)
    {
        free(wordList[i]);
    }

    return 0;
}
void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

int findYulMyungPaths(const char *yul, const char *myung, char **wordList, int wordListSize)
{
    Queue *q = cQueue();
    Graph *graph = cGraph(wordListSize + 2);
    int Yulindex = wordListSize;
    int Myungindex = wordListSize + 1;

    wordList[Yulindex] = strdup(yul);
    wordList[Myungindex] = strdup(myung); // yul, myung 추가

    int Exist = 0;
    for (int i = 0; i < wordListSize; i++)
    {
        if (strcmp(wordList[i], myung) == 0)
        {
            Exist = 1;
            break;
        }
    }

    if (!Exist)
    {
        return 0; // myung이 없을 경우
    }

    for (int i = 0; i < wordListSize + 2; i++)
    {
        for (int j = i + 1; j < wordListSize + 2; j++)
        {
            if (Convertable(wordList[i], wordList[j]))
            {
                addEdge(graph, i, j); // 간선 추가
            }
        }
    }

    int *distance = malloc((wordListSize + 2) * sizeof(int));
    int *path = malloc((wordListSize + 2) * sizeof(int));

    for (int i = 0; i < wordListSize + 2; i++)
    {
        distance[i] = -1;
        path[i] = 0;
    }

    distance[Yulindex] = 0;
    path[Yulindex] = 1; // 시작점 설정

    enqueue(q, Yulindex);

    while (Filled(q))
    {
        int current = dequeue(q);
        Node *neighbor = graph->connections[current];
        while (neighbor != NULL)
        {
            if (distance[neighbor->idx] == -1)
            {
                distance[neighbor->idx] = distance[current] + 1;
                path[neighbor->idx] = path[current];
                enqueue(q, neighbor->idx);
            }
            else if (distance[neighbor->idx] == distance[current] + 1)
            {
                path[neighbor->idx] += path[current];
            }
            neighbor = neighbor->next;
        }
    } // BFS 탐색

    int YulMyungPaths = path[Myungindex];

    free(graph);
    free(distance);
    free(path);

    return YulMyungPaths;
}

