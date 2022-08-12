// -----------------------
#include <stdio.h>
#include <stdlib.h>

// Simple node
struct node {
  int value;  
  
  struct node *leftNode;
  struct node *rightNode;
};
typedef struct node node_t;  // makes it easier for coding below

// Return bit representation of the byte passed in
char *byte2Bits(unsigned char aByte) {
  static char rtnString[9] = "00000000";
  unsigned char mask;
  char i;
  for (i = 7, mask = 1; i >= 0; i--) {
    rtnString[i] = (aByte & mask) ?  '1' :'0';
    mask<<=1;
  }  
  return rtnString;
}

// Create a node and return it
node_t *createNode(int value) {
  node_t *newNode = malloc(sizeof(node_t));
  newNode->value = value;
  
  newNode->leftNode = NULL;
  newNode->rightNode = NULL;
  return newNode;
}

// My own pow() routine for ints
unsigned int intPower(int base, int toPower) {
  if (toPower == 0) return 1;
  else 
    return (intPower(base,toPower-1)*base);
}

// Output node, we pass it's address in hear otherwise a copy of it would have been made
void printNode(char *msg, node_t **ptr) {
  printf("%20s, &node: %p, node: %p, ",msg,ptr,*ptr);
  if (*ptr != NULL) {
    printf("node->value: %d, ",(*ptr)->value);

    if ((*ptr)->leftNode != NULL) {
      printf("node->leftNode: %p (%d), ",(*ptr)->leftNode,(*ptr)->leftNode->value);
    }

    if ((*ptr)->rightNode != NULL) {
      printf("node->rightNode: %p (%d)",(*ptr)->rightNode,(*ptr)->rightNode->value);
    }
  }
  printf("\n");
}

// Swap nodes
void swapNodesByReference(node_t **a, node_t **b) {
  node_t *temp = *a;
  *a = *b;
  *b = temp;
}

// Swap nodes
void swapNodesByValue(node_t *a, node_t *b) {
  node_t *temp = a;
  a = b;
  b = temp;
}

// Swap left and right nodes
void swapChildrensNodes(node_t *a) {
  node_t *temp = a->leftNode;
  a->leftNode = a->rightNode;
  a->rightNode = temp;
}

// ------------------------------------------------------------------
int main() {
  // Notes define pointer with <type> *varName
  // Assign value to pointer with var = &someVarOf<type>
  // Using varName will give address of element it contains
  // You dereference it with *varName, it then gives you the value it's pointing to

  int x = 8;
  int *xPtr;
  xPtr = &x;
  printf(" x has value: %d\n *xPtr also has: %d\n address of xPtr: %p\n",x,*xPtr,xPtr);

  // When you define an array, the array is a pointer to where it starts, it's the same as &array[0]
  // Example below shows you can use pointer to iterate over the array
  int someNumbers[5];
  int *somePtr;
  int someValue = 2;
  for (somePtr = someNumbers; somePtr != &someNumbers[5]; somePtr++) {
    *somePtr = someValue;
    someValue *= 2;
  }

  for (int i = 0; i < 5; i++) {
   printf("value at someNumbers[%d]: %d\n",i,someNumbers[i]);
  }

  // Proof that someNumbers and &someNumbers[0] point to same thing
  printf("\nsomeNumbers address: %p  &someNumbers[0]: %p\n",someNumbers,&someNumbers[0]);
  printf("*someNumbers value: %d someNumbers[0] value: %d\n",*someNumbers,someNumbers[0]);

  // Use pointer to inspect different bytes of an integer and show binary representation
  someNumbers[0] = 7;
  someNumbers[1] = 256 + 3;
  someNumbers[2] = 16909320;
  // This shows each byte of a 4 byte integer
  unsigned char *cPtr;
  unsigned int toPower;
  for (int j = 0; j < 3; j++) {
    cPtr = &someNumbers[j];
    printf("Showing little endian format of: %d\n",someNumbers[j]);

    for (int i = 0; i < 4; i++) {
      toPower = intPower(256,i)* *cPtr;
      printf("  byte: %d,  contains: %u  binary: %s  pow(256,%d)*%u:%11d\n",i,*cPtr,byte2Bits(*cPtr),i,*cPtr,toPower);
      cPtr++;
    }
  }

  // -----------------------------------------------------------------------------------------------------------
  if (1 == 0) {
    printf("\n\nPowers\n");
    // Show powers
    unsigned int idx, tempVar;
    tempVar = 1;
    for (idx = 0; idx < 32; idx++) {
      printf("  pow(2,%d): %u\n",idx,tempVar);
      tempVar<<=1;
    }
  }

  //-----------------------------------------------------------------------------------
  //   T E S T   B Y T E   T O   B I N A R Y   C O N V E R S I O N
  //-----------------------------------------------------------------------------------
  if (1 == 0) {
    printf("\n\nTest showing byte2Bits conversions\n");
    unsigned char dum2; // byte to show
    int dum1;           // for iterating, want to see wrap so need bigger than a byte here
    for (dum1 = 0, dum2 = 0; dum1 < 257; dum1++, dum2++) {
      printf("%4d converted by bytes: %s\n",dum2,byte2Bits(dum2));    
    }
  }

  // More with pointers and pass by ref/value, understand this
  if (1 == 1) {
    node_t *head      = NULL;
    node_t *headLeft  = NULL;
    node_t *headRight = NULL;   

    printf("Mainline: &head: %p, &headLeft: %p,  &headRight: %p\n",&head,&headLeft,&headRight);
    
    printNode("Head start",&head);
    head            = createNode(5);    
    headLeft        = createNode(3);
    head->leftNode  = headLeft;
    headRight       = createNode(8);
    head->rightNode = headRight;
    printNode("Head assigned",&head);
    
    printNode("headLeft",&headLeft);
    printNode("headRight",&headRight);

    // This calls the swap routine but the nodes are passed in by value so the swap doesn't do anything
    swapNodesByValue(headLeft,headRight);
    printf("swapNodesByValue(headLeft,headRight)\n");
    printNode("headLeft",&headLeft);
    printNode("headRight",&headRight);

    // This one we are changing the children of the node passed in so the change takes place
    swapChildrensNodes(head);
    printf("swapChildrensNodes(head)\n");
    printNode("head",&head);

    // Here we show the proper way we need to change the pointer values and have them persist
    // Understand that head's leftNode and rightNode aren't affected by swapping headLeft/headRight
    //   since leftNode/rightNode contain pointers... with the swap it's really the 
    //   value stored at &headLeft/&headRight that's changing
    swapNodesByReference(&headLeft,&headRight);
    printf("swapNodesByReference(&headLeft,&headRight)\n");
    printNode("headLeft",&headLeft);
    printNode("headRight",&headRight);
    printNode("head",&head);
  }
  
}
