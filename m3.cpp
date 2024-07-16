#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the structure for a linked list node
struct LinkedListNode {
    char* name;
    struct LinkedListNode* next;
};

// Function to create a new node
struct LinkedListNode* createNode(const char* name) {
    struct LinkedListNode* newNode = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
    if (newNode != NULL) {
        newNode->name = (char*)malloc(strlen(name) + 1); // Allocate memory for the name
        if (newNode->name != NULL) {
            strcpy_s(newNode->name, strlen(name) + 1, name); // Copy the name into the allocated memory
        }
        newNode->next = NULL;
    }
    return newNode;
}

// Function to insert a node into a sorted linked list
void insertSorted(struct LinkedListNode** head, const char* name) {
    struct LinkedListNode* newNode = createNode(name);
    if (newNode == NULL) {
        printf("Error allocating memory for new node.\n");
        exit(1);
    }
    if (*head == NULL || strcmp((*head)->name, name) > 0) {
        newNode->next = *head;
        *head = newNode;
    }
    else {
        struct LinkedListNode* current = *head;
        while (current->next != NULL && strcmp(current->next->name, name) < 0) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Function to search a name in a linked list
struct LinkedListNode* searchLinkedList(const char* searchName, struct LinkedListNode* linkedList, int* comparisonCount) {
    struct LinkedListNode* current = linkedList;
    *comparisonCount = 0;
    while (current != NULL) {
        (*comparisonCount)++;
        if (strcmp(current->name, searchName) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Function to search for a name in both the hash table and the linked list
void searchForNameTwice(const char* searchName, struct LinkedListNode* hashTable[], int comparisonCount[2], struct LinkedListNode* linkedList) {
    int listComparisonCount;
    struct LinkedListNode* result;

    // Search the linked list
    result = searchLinkedList(searchName, linkedList, &listComparisonCount);
    comparisonCount[0] = listComparisonCount;
    if (result != NULL) {
        printf("\t%s was found in the linked list in %d comparisons\n", searchName, listComparisonCount);
    }
    else {
        printf("\t%s was NOT found in the linked list in %d comparisons\n", searchName, listComparisonCount);
    }

    // Hash function to determine the bucket
    int hashIndex = 0;
    for (int i = 0; searchName[i] != '\0'; i++) {
        hashIndex = (hashIndex + searchName[i]) % 127;
    }

    // Search the hash table bucket
    result = searchLinkedList(searchName, hashTable[hashIndex], &listComparisonCount);
    comparisonCount[1] = listComparisonCount;
    if (result != NULL) {
        printf("\t%s was found in the hash table bucket in %d comparisons\n", searchName, listComparisonCount);
    }
    else {
        printf("\t%s was NOT found in the hash table bucket in %d comparisons\n", searchName, listComparisonCount);
    }
}

int main() {
    // Read names from the file
    FILE* file;
    if (fopen_s(&file, "names.txt", "r") != 0) {
        printf("Error opening file.\n");
        return 1;
    }

    char name[31];
    struct LinkedListNode* linkedList = NULL;
    struct LinkedListNode* hashTable[127] = { NULL };

    while (fgets(name, sizeof(name), file)) {
        // Remove newline character
        size_t len = strlen(name);
        if (len > 0 && name[len - 1] == '\n') {
            name[len - 1] = '\0';
        }

        // Insert into the linked list
        insertSorted(&linkedList, name);

        // Hash function to determine the bucket
        int hashIndex = 0;
        for (int i = 0; name[i] != '\0'; i++) {
            hashIndex = (hashIndex + name[i]) % 127;
        }

        // Insert into the hash table bucket
        insertSorted(&hashTable[hashIndex], name);
    }

    fclose(file);

    char searchName[31];
    int comparisonCount[2];
    int totalSearches = 0;
    int totalListComparisons = 0;
    int totalHashTableComparisons = 0;

    while (1) {
        printf("Enter a name to search (or 'exit' to quit): ");
        fgets(searchName, sizeof(searchName), stdin);

        // Remove newline character
        size_t len = strlen(searchName);
        if (len > 0 && searchName[len - 1] == '\n') {
            searchName[len - 1] = '\0';
        }

        if (strcmp(searchName, "exit") == 0) {
            break;
        }

        searchForNameTwice(searchName, hashTable, comparisonCount, linkedList);
        totalSearches++;
        totalListComparisons += comparisonCount[0];
        totalHashTableComparisons += comparisonCount[1];
    }

    printf("\n\n\n");
    printf("Total searches: %d\n", totalSearches);
    printf("Total comparisons in sorted linked list: %d\n", totalListComparisons);
    printf("Total comparisons in hash table: %d\n", totalHashTableComparisons);

    // Clean up allocated memory
    struct LinkedListNode* current, * temp;

    current = linkedList;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp->name);
        free(temp);
    }

    for (int i = 0; i < 127; i++) {
        current = hashTable[i];
        while (current != NULL) {
            temp = current;
            current = current->next;
            free(temp->name);
            free(temp);
        }
    }

    return 0;
}
