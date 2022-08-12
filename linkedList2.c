#include <stdio.h>
#include <stdlib.h>

// Link list examples
struct node {
  int value;
  struct node* next;
};
typedef struct node node_t; // makes it easier for coding below

// Create a new now with a given value and return it's pointer
node_t *createNode(int value) {
  node_t *newNode = malloc(sizeof(node_t));
  newNode->value = value;
  newNode->next = NULL;
  return newNode;
}

// Return node with given value, if not found NULL is returned
node_t *findNode(node_t *nodePtr, int value) {
  if (nodePtr == NULL || nodePtr->value == value) {
    return nodePtr;
  }
  else {
    return findNode(nodePtr->next,value);
  }
} 

// Need to modify headPtr so we pass the address of it in, that's why **
void insertValue(node_t **headPtr, int value) {
  node_t *newNode = createNode(value);   
  node_t *parentPtr = NULL;
  node_t *currentNode = *headPtr;
  while (currentNode != NULL && currentNode->value < newNode->value) {
    parentPtr = currentNode;
    currentNode = currentNode->next;
  }

  if (parentPtr == NULL) { // Add to top of list
    newNode->next = *headPtr;   // new nodes next points to old headPtr;
    *headPtr = newNode;
  }
  else {
    parentPtr->next = newNode;
    newNode->next = currentNode;
  }
}    

// Shows routine to print the list, the first block uses recursion to do it,
// the block below it uses a while loop, note that c always uses call by value
// so the ptr passed in is not corrupted upon return (it has the same value)
void printList(node_t *ptr) {
  if (1 == 0) {
    printf("Pointer address: %p  Value:  %d\n",&ptr,ptr->value);
    if (ptr->next != NULL) {
      printList(ptr->next);
    }
  }
  else {
    while (ptr != NULL) {
      printf("Pointer address is: %p   has address: %p  points->value: %d\n",&ptr,ptr,ptr->value);
      ptr = ptr->next;
    }
  }
}

// We are passed the address of a pointer here
void testIt(node_t **aPtr) {
  printf("aPtr is %p (this is address of pointer passed in)\n",aPtr);
  printf("*aPtr is %p (this is address of what aPtr is pointing to\n",*aPtr);
  printf("(*aPtr)->value is %d\n",(*aPtr)->value);
}

// ---------------------------------------------------------------------------------------------- 
int main() {
  node_t *head = NULL;

  insertValue(&head, 45);
  insertValue(&head, 20);
  insertValue(&head, 30);
  insertValue(&head, 55);
  printList(head);

  printf("found 30 at %p\n",findNode(head,30));
  printf("found 31 at %p\n",findNode(head,31));

  testIt(&head);
  printf("&head is: %p  head is: %p\n",&head,head);
}
