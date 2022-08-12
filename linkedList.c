#include <stdio.h>
#include <stdlib.h>

// Link list examples
struct node {
  int value;
  struct node* next;
};
typedef struct node node_t; // makes it easier for coding below

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

// Simple way to print reverse order
void printReverse(node_t *ptr) {
  if (ptr != NULL) {
    printReverse(ptr->next);
    printf("printReverese: &ptr: %p   has address: %p  points->value: %d\n",&ptr,ptr,ptr->value);
  }  
}


// ----------------------------------------------------- 
int main() {
  node_t n1, n2, n3;
  node_t *head;

  n1.value = 45;
  n1.next  = &n2;

  n2.value = 50;
  n2.next  = &n3;

  n3.value = 100;
  n3.next  = NULL;

  head = &n1;
  while (head != NULL) {
    printf("Value: %d\n",head->value);
    head = head->next;
  }

  // Shows that the pointer value is preserved even after calling the
  // printList function.
  head = &n1;
  printf("&n1 is %p\n",&n1);

  printf("Head is stored at %p   Head contains address: %p\n",&head,head);
  printList(head);
  printf("head address after: %p\n",head);

  printReverse(head);
}

