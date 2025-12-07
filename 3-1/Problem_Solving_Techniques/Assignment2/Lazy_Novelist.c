#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Trie
{
    struct Trie *child[26];
    char *word;
    char *best_word;
    int importance;
    int best_importance;
} Trie;

Trie *createTrie()
{
    Trie *node = (Trie *)malloc(sizeof(Trie));
    node->importance = 0;
    node->word = NULL;
    node->best_word = NULL;
    node->best_importance = 0;
    for (int i = 0; i < 26; i++)
    {
        node->child[i] = NULL;
    }
    return node;
}

void insert(Trie *root, char *word, int importance)
{
    Trie *current = root;
    char *input = word;
    int index;

    current->importance = importance;

    while (*word)
    {
        index = *word - 'A';
        if (current->child[index] == NULL)
            current->child[index] = createTrie();

        current = current->child[index];

        if (importance > current->best_importance || (importance == current->best_importance && strcmp(input, current->best_word) < 0))
        {
            current->best_importance = importance;
            current->best_word = strdup(input);
        }
        word++;
    }
    current->word = strdup(input);
}

int How_Many_Type(Trie *root, char *word)
{
    Trie *current = root;
    int count = 0;
    int i = 0;
    int length = strlen(word);

    while (word[i] != '\0')
    {
        current = current->child[word[i] - 'A'];
        if (current == NULL)
        {
            break;
        }
        count++;
        if (current->best_word && strncmp(current->best_word, word, strlen(current->best_word)) == 0 && strlen(current->best_word) == strlen(word))
        {
            if (i == length - 1)
            {
                i++;
                break;
            }

            return count + 1;
        }
        i++;
    }

    count += strlen(word) - i;
    return count;
}

int total_Type(Trie *root, char *sentence)
{
    int total = 0;
    int i = 0;
    int j = 0;
    char currentWord[7];

    while (sentence[i] != '\0')
    {
        if (sentence[i] == ' ')
        {
            currentWord[j] = '\0';
            total += How_Many_Type(root, currentWord);
            total++;
            j = 0;
        }
        else
        {
            currentWord[j] = sentence[i];
            j++;
        }
        i++;
    }

    currentWord[j] = '\0';
    total += How_Many_Type(root, currentWord);

    return total;
}

int main()
{
    int N, M;
    int importance;
    char word[7];
    char sentence[100000];
    int total;

    scanf("%d %d", &N, &M);


    Trie *root = createTrie();

    for (int i = 0; i < N; i++)
    {
        scanf("%s %d", word, &importance);
        insert(root, word, importance);
    }
    
    scanf(" %[^\n]", sentence);

    total = total_Type(root, sentence);

    printf("%d", total);

    return 0;
}

