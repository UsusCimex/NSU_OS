#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct _Node {
    char value[100];
    struct _Node* next;
    pthread_mutex_t sync;
} Node;

typedef struct _Storage {
    Node* first;
} Storage;

int increasing_length_count = 0;
int decreasing_length_count = 0;
int equal_length_count = 0;
int swap_count = 0;

pthread_mutex_t global_mutex;

void addNode(Storage* storage, const char* value) {
    Node* new_node = (Node* )malloc(sizeof(Node));
    strncpy(new_node->value, value, 100);
    new_node->value[99] = '\0'; // Обеспечиваем, что строка будет нуль-терминированной
    pthread_mutex_init(&new_node->sync, NULL);
    new_node->next = storage->first;
    storage->first = new_node;
}

void deleteNode(Storage* storage, Node* node) {
    if (storage->first == NULL || node == NULL) return;

    if (storage->first == node) {
        storage->first = node->next;
    } else {
        Node* temp = storage->first;
        while (temp->next != NULL && temp->next != node) {
            temp = temp->next;
        }
        if (temp->next == node) {
            temp->next = node->next;
        }
    }
    pthread_mutex_destroy(&node->sync);
    free(node);
}

// Функция потока для подсчета пар строк, идущих по возрастанию длины
void* countIncreasingLengthPairs(void* arg) {
    Storage* storage = (Storage*)arg;
    while (1) {
        Node* current = storage->first;
        Node* next = NULL;

        while (current != NULL) {
            pthread_mutex_lock(&current->sync);
            next = current->next;
            if (next != NULL) {
                pthread_mutex_lock(&next->sync);
                if (strlen(current->value) < strlen(next->value)) {
                    increasing_length_count++;
                }
                pthread_mutex_unlock(&next->sync);
            }
            pthread_mutex_unlock(&current->sync);
            current = next;
        }
    }
    return NULL;
}

// Функция потока для подсчета пар строк, идущих по убыванию длины
void* countDecreasingLengthPairs(void* arg) {
    Storage* storage = (Storage* )arg;
    while (1) {
        Node* current = storage->first;
        Node* next = NULL;

        while (current != NULL) {
            pthread_mutex_lock(&current->sync);
            next = current->next;
            if (next != NULL) {
                pthread_mutex_lock(&next->sync);
                if (strlen(current->value) > strlen(next->value)) {
                    decreasing_length_count++;
                }
                pthread_mutex_unlock(&next->sync);
            }
            pthread_mutex_unlock(&current->sync);
            current = next;
        }
    }
    return NULL;
}

// Функция потока для подсчета пар строк одинаковой длины
void* countEqualLengthPairs(void* arg) {
    Storage* storage = (Storage*)arg;
    while (1) {
        Node* current = storage->first;
        Node* next = NULL;

        while (current != NULL) {
            pthread_mutex_lock(&current->sync);
            next = current->next;
            if (next != NULL) {
                pthread_mutex_lock(&next->sync);
                if (strlen(current->value) == strlen(next->value)) {
                    equal_length_count++;
                }
                pthread_mutex_unlock(&next->sync);
            }
            pthread_mutex_unlock(&current->sync);
            current = next;
        }
    }
    return NULL;
}

void swapNodes(Storage* storage, Node* node1, Node* node2) {
    if (node1 == NULL || node2 == NULL || node1 == node2) return;

    // Находим предшествующие узлы для node1 и node2
    Node *prev1 = NULL, *prev2 = NULL, *curr = storage->first;
    while (curr != NULL && (prev1 == NULL || prev2 == NULL)) {
        if (curr->next == node1) prev1 = curr;
        if (curr->next == node2) prev2 = curr;
        curr = curr->next;
    }

    // Обновляем предшествующие узлы
    if (prev1 != NULL) prev1->next = node2;
    if (prev2 != NULL) prev2->next = node1;

    // Обмениваем следующие узлы
    Node* temp = node1->next;
    node1->next = node2->next;
    node2->next = temp;

    // Обновляем первый элемент списка, если необходимо
    if (storage->first == node1) storage->first = node2;
    else if (storage->first == node2) storage->first = node1;
}

// Функция потока для перестановки узлов списка
void* swapNodesInList(void* arg) {
    Storage* storage = (Storage*)arg;
    while (1) {
        Node* current = storage->first;
        Node* next = NULL;

        while (current != NULL) {
            pthread_mutex_lock(&global_mutex); // To avoid deadlock
            pthread_mutex_lock(&current->sync);
            next = current->next;
            if (next != NULL) {
                pthread_mutex_lock(&next->sync);
                if (next != NULL && rand() % 100 < 25) { // 25% to swap
                    swapNodes(storage, current, next);
                    swap_count++;
                }
                pthread_mutex_unlock(&next->sync);
            }
            pthread_mutex_unlock(&current->sync);
            current = next;
            pthread_mutex_unlock(&global_mutex);
        }
    }
    return NULL;
}

void* monitor(void* arg) {
    while (1) {
        printf("increasing_length_count = %d\ndecreasing_length_count = %d\nequal_length_count = %d\nswap_count = %d\n",
                increasing_length_count,
                decreasing_length_count,
                equal_length_count,
                swap_count);
        sleep(1);
    }
}

void generateRandomString(char *str, int length) {
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < length - 1; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[length - 1] = '\0';
}

void generateList(Storage *storage, int numNodes) {
    for (int i = 0; i < numNodes; i++) {
        char randomString[100];
        generateRandomString(randomString, rand() % 100);
        addNode(storage, randomString);
    }
}

int main() {
    Storage storage;
    storage.first = NULL;

    generateList(&storage, 10000);

    pthread_t inc_thread, dec_thread, eq_thread, swap_thread1, swap_thread2, swap_thread3, monitor_thread;
    pthread_create(&inc_thread, NULL, countIncreasingLengthPairs, (void* )&storage);
    pthread_create(&dec_thread, NULL, countDecreasingLengthPairs, (void* )&storage);
    pthread_create(&eq_thread, NULL, countEqualLengthPairs, (void* )&storage);
    pthread_create(&swap_thread1, NULL, swapNodesInList, (void* )&storage);
    pthread_create(&swap_thread2, NULL, swapNodesInList, (void* )&storage);
    pthread_create(&swap_thread3, NULL, swapNodesInList, (void* )&storage);
    pthread_create(&monitor_thread, NULL, monitor, (void* )&storage);

    // Дождаться завершения потоков (в данном случае, потоки выполняются бесконечно)
    pthread_join(inc_thread, NULL);
    pthread_join(dec_thread, NULL);
    pthread_join(eq_thread, NULL);
    pthread_join(swap_thread1, NULL);
    pthread_join(swap_thread2, NULL);
    pthread_join(swap_thread3, NULL);
    pthread_join(monitor_thread, NULL);

    return 0;
}
