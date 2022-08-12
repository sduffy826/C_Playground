// ----------------------------------------------------------------------------
// This is a little program to show a binary tree, it has various utility 
// functions and shows how to balance the tree etc... 
// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0
#define REALTIMEBALANCE 1  // If 1 we'll rebalance during updates

// Constants for identifying type of inbalance, just for readability
#define INBALANCE 0
#define LEFT 1
#define RIGHT 2
#define LEFTRIGHT 3
#define RIGHTLEFT 4

// Node for a tree
struct node {
  int value;
  struct node *parentNode;
  struct node *leftNode;
  struct node *rightNode;
};
typedef struct node node_t; // makes it easier for coding below

// ====================================================================
// Forward declarations
node_t *findNode(node_t *, int);
int getBalanceValue(node_t *);
void isHeavy(node_t *, int *, int *);
void insertNodeHelper(node_t **, node_t *, node_t *);
int leftRightNumNodes(node_t *, int *, int *);
int max(int, int);
void printNode(char *, node_t **);
void rebalanceNode(node_t *);
void rebalanceTreeUpFrom(node_t *, node_t **);

// --------------------------------------------------------------------
// Returns 1 if all children in balance, 0 otherwise
int allChildrenInBalance(node_t *ptr) {
  if (ptr == NULL) return 1;

  // Check that leftNode and rightNodes are in balance
  if ((getBalanceValue(ptr->leftNode) != INBALANCE) || 
      (getBalanceValue(ptr->rightNode) != INBALANCE)) {
    return 0;
  }
  else {
    // Now we'll recursively call to check our children
    if ((allChildrenInBalance(ptr->leftNode) != 1) ||
        (allChildrenInBalance(ptr->rightNode) != 1)) {
      return 0;
    }
    else {
      return 1;
    }
  }
}

// Helper to check pointers
void checkParentPointersHelper(node_t **parent, node_t **child) {
  if (DEBUG == 3) printf("checkParentPointersHelper *parent: %p *child: %p\n",*parent,*child);
  if (*child != NULL) {
    if ((*child)->parentNode != *parent) {
      printf("** Child pointing to wrong parent, parent should be: %d **\n",(*parent)->value);
      printNode("Parent: ",parent);
      printNode("Child:  ",child);
    }
  }
}

// Check that all children point back to their parents, uses helper above
void checkParentPointers(node_t **ptr) {
  if (*ptr != NULL) {
    // Call helper to check parent/child pointers
    checkParentPointersHelper(ptr,&((*ptr)->leftNode));
    checkParentPointersHelper(ptr,&((*ptr)->rightNode));   
    
    // Recursively call children to check them
    checkParentPointers(&((*ptr)->leftNode));
    checkParentPointers(&((*ptr)->rightNode));
  }
}

// Create a new node with a given value, we'll return it's pointer
node_t *createNode(int value) {
  node_t *newNode = malloc(sizeof(node_t));
  newNode->value = value;
  newNode->leftNode = NULL;
  newNode->rightNode = NULL;
  return newNode;
}


// Delete node with a given value, we may need to reset headPtr so it's passed by ref here
void deleteValue(node_t **headPtr, int value) {
  node_t *node2Delete = findNode(*headPtr,value);
  if (DEBUG) {
    printNode("\ndeleteValue",&node2Delete);
    printNode("headPtr:",headPtr);
  }
  if (node2Delete != NULL) {
    node_t *parentNode = node2Delete->parentNode;
    
    node_t **replacementNode;      // The helper needs a pointer that points to a node_t
    node_t *rebalanceUpFrom;       // If we want realtime rebalance then we'll do it from this node up
    node_t *node2ReInsert = NULL;  // Node to re insert into the tree
    rebalanceUpFrom       = NULL;  // Node that we'll rebalance from

    // if either of deleted nodes children are null then move the corresponding node up, this is simple case
    if ((node2Delete->leftNode == NULL) || (node2Delete->rightNode == NULL)) {
      replacementNode = (node2Delete->leftNode == NULL) ? &(node2Delete->rightNode) : &(node2Delete->leftNode);
      if (*replacementNode != NULL) {
        (*replacementNode)->parentNode = node2Delete->parentNode;
        rebalanceUpFrom = *replacementNode;
      }
      else {
        // We don't have a replacement node taking the deleted nodes position, we'll rebalance up from
        // the deleted nodes parent.
        rebalanceUpFrom = node2Delete->parentNode;
      }
    }
    else {
      // Both children of the deleted node are defined, we'll use the right child to replace the deleted node
      // and call insertHelper to reattach it's children.
      replacementNode = &(node2Delete->rightNode);
      node2ReInsert   = (*replacementNode)->leftNode;     // Node that needs to be re-inserted
      if (node2ReInsert != NULL) {                        // Clear the parentNode to be re-inserted
        node2ReInsert->parentNode = NULL;
      }
      (*replacementNode)->leftNode = node2Delete->leftNode;           // Have leftNode point to deletedNodes leftNode
      if ((*replacementNode)->leftNode != NULL) {                     // If leftNode isn't null then reset parent
        (*replacementNode)->leftNode->parentNode = *replacementNode;  // It was pointed to deletedNode, has to point to replacement
        if (DEBUG) printNode("leftChildFix",&((*replacementNode)->leftNode));
      }
      (*replacementNode)->parentNode = node2Delete->parentNode; // Set parent
      // We didn't reInsert node so we'll rebalance up from the replacement node position, later we may change this
      rebalanceUpFrom = *replacementNode;      
    }
    // We've now replaced the deleted node with the replacement node

    // Set pointer in parent of deleted node to point to the replacement node
    if (parentNode != NULL) {
      if (parentNode->leftNode == node2Delete) 
        parentNode->leftNode = *replacementNode;
      else 
        parentNode->rightNode = *replacementNode;
    }

    // If the deleted node was the head then we reset to the replacementNode
    if (node2Delete == *headPtr) {
      *headPtr = *replacementNode;
      if (DEBUG) printNode("reset headPtr:",headPtr);
    }

    // If we need to re-attach node do it now, this will reset the rebalanceUpFrom 
    if (node2ReInsert != NULL) {
      if (DEBUG) printNode("node2ReInsert:",&node2ReInsert);
    
      insertNodeHelper(replacementNode,(*replacementNode)->parentNode,node2ReInsert);  // Reinsert node 
      rebalanceUpFrom = node2ReInsert;
    }

    if (DEBUG) printNode("replacementNode: ",replacementNode);

    if (REALTIMEBALANCE) {
      if (DEBUG) printNode("rebalanceUpFrom: ",&rebalanceUpFrom);
      if (DEBUG) printNode("headPtr:",headPtr);
      rebalanceTreeUpFrom(rebalanceUpFrom,headPtr);
    }

    // Free up space of the deleted node
    free(node2Delete);
  }
}


// Determine depth of a node, note: 
//   The depth of a NULL node is -1
//   Depth of a node with no children is 0
//   Otherwise it's the max depth of lineage (1 children, 2 grandchildren, 3 great grandchildren etc...)
int depth(node_t *ptr) {
  if (ptr == NULL) {
    return -1;
  }
  else {
    return max(1 + depth(ptr->leftNode),1 + depth(ptr->rightNode));
  }
}


// Return node with given value, if not found NULL is returned
node_t *findNode(node_t *nodePtr, int value) {
  if (nodePtr == NULL || nodePtr->value == value) {
    return nodePtr;
  }
  else 
    {
      if (nodePtr->value > value) {
        return findNode(nodePtr->leftNode,value);
      }
      else {
        return findNode(nodePtr->rightNode,value);
      }
    }
} 


// Return a string representation of the balance value passed in
char *getBalanceStringValueHelper(int balanceValue) { 
  switch(balanceValue) {
    case LEFT:
      return "LEFT"; 
      break;  
    case RIGHT:
      return "RIGHT";
      break;  
    case LEFTRIGHT: 
      return "LEFT/RIGHT";
      break;  
    case RIGHTLEFT:
      return "RIGHT/LEFT";
      break;  
    default:
      return "INBALANCE";
  }
}


// Return a string representation of the balance 
char *getBalanceStringValue(node_t *ptr) { 
  return getBalanceStringValueHelper(getBalanceValue(ptr));
} 


// Returns balance for a particular node, logic:
//   Determine if node is outOfBalance, if it is then see if child is heavy
//   to the opposite side... set return value accordingly
int getBalanceValue(node_t *ptr) {
  int heavySide, balanceType;
  isHeavy(ptr, &heavySide, &balanceType);  // This call only cares about balanceType
  switch(balanceType) {
    case INBALANCE :
      return balanceType;
      break;
    case LEFT:
      isHeavy(ptr->leftNode,&heavySide,&balanceType);
      if (heavySide == RIGHT) {
        return LEFTRIGHT;
      }
      else {
        return LEFT;
      }
      break;
    case RIGHT:
      isHeavy(ptr->rightNode,&heavySide,&balanceType);
      if (heavySide == LEFT) {
        return RIGHTLEFT;
      }
      else {
        return RIGHT;
      }
      break;
  }
}


// Get nodes parent, this function DOES NOT use parentNode.  If you don't want
// to maintain parentNode then you could use this routine to get any nodes parent.
// Also: the last arg will cause it to search using the nodes value, that will make
//       the search more efficient... if you don't use that then it may search
//       the entire tree 
node_t *getNodesParent(node_t *node2Check, node_t *node2Find, char smartSearch) {
  if (DEBUG) printf(">> getNodesParent ");
  if (node2Check == NULL || node2Check == node2Find  || node2Find == NULL) {
    // If we are passed in the node to find then it has no parent, return null
    return NULL;
  }
  else 
    if (node2Check->leftNode == node2Find  || node2Check->rightNode == node2Find)  {
      return node2Check;
    }
    else {
      // If smartSearch is on then we'll look at the values of the nodes to determine search path
      if (smartSearch) {
        if (node2Check->value < node2Find->value) {
          return getNodesParent(node2Check->rightNode,node2Find,smartSearch);
        }
        else {
          return getNodesParent(node2Check->leftNode,node2Find,smartSearch);
        }
      }
      else {
        // smartSearch is off, search entire tree till found
        node_t *tempPtr = getNodesParent(node2Check->leftNode, node2Find, smartSearch);
        if (tempPtr != NULL) {
          return tempPtr;
        }
        else {
          return getNodesParent(node2Check->rightNode, node2Find, smartSearch);
        }
      }
    }
}


// Get the node that needs balancing, it'll return the node at the lowest level in the tree.  Since
// we need to update the pointer and level it's at they are passed in by reference
void getNodeToBalance(node_t *node2Check, int currLevel, node_t **node2Balance, int *node2BalanceLevel) {
  if (DEBUG  == 2) {
    printf("getNodeToBalance node2Check: %p currLevel: %d  node2Balance: %p *node2Balance: %p  *node2BalanceLevel: %d\n",
           node2Check, currLevel, node2Balance, *node2Balance, *node2BalanceLevel);
  }
   
  if (node2Check != NULL) { 
    if (currLevel > *node2BalanceLevel) {
      if (getBalanceValue(node2Check) != INBALANCE) {
        *node2Balance      = node2Check;
        *node2BalanceLevel = currLevel;
      }
    }

    getNodeToBalance(node2Check->leftNode,currLevel+1,node2Balance,node2BalanceLevel);
    getNodeToBalance(node2Check->rightNode,currLevel+1,node2Balance,node2BalanceLevel);
  }
} 


// Helper routine to insert a node into the tree, we update the nodePtr value so it's passed by ref
void insertNodeHelper(node_t **nodePtr, node_t *parentPtr, node_t *node2Insert) {
  if (DEBUG) {
    printNode("\ninsertNodeHelper nodePtr    ",nodePtr);
    printNode("insertNodeHelper parentPtr  ",&parentPtr);
    printNode("insertNodeHelper node2Insert",&node2Insert);
  }
  // If nodePtr is null then insert new node here and set it's parent node, we're done :)
  if (*nodePtr == NULL) {
    *nodePtr = node2Insert;
    (*nodePtr)->parentNode = parentPtr;

    // If we want real time balancing then we call rebalanceNode for this nodes grandparent
    if (REALTIMEBALANCE && parentPtr != NULL && parentPtr->parentNode != NULL) {
      if (DEBUG) {
        printNode("callREBALANCENODE(1)",&parentPtr);
        printNode("callREBALANCENODE(2)",&(parentPtr->parentNode));
      }
      rebalanceNode(parentPtr->parentNode);
    }
  }
  else {
    // If value in nodePtr is less than the value we want to insert then 
    // inspect the rightNode else inspect leftNode
    if ((*nodePtr)->value < node2Insert->value) {
      insertNodeHelper(&((*nodePtr)->rightNode),*nodePtr,node2Insert);
    }
    else {
      insertNodeHelper(&((*nodePtr)->leftNode),*nodePtr,node2Insert);
    }
  }
}


// Routine to insert a value into the tree, bulk of work is done in the helper (insertNodeHelper)
void insertValue(node_t **headPtr, int value) {
  if (findNode(*headPtr,value) == NULL) {   // Only insert if not found
    node_t *newNode = createNode(value);
    insertNodeHelper(headPtr, NULL, newNode);

    // We may have rebalanced the tree the headPtr's parent might no longer be null, that means that
    // it's parent is now the head
    if ((*headPtr)->parentNode != NULL) {
      *headPtr = (*headPtr)->parentNode;
      if (DEBUG) printf("\nRepositioning head to %p\n",*headPtr);
    }    
  }
  else {
    printf("You asked to insert %d but we already had it, no action taken\n",value);
  }
} 


// Little helper that determines if node is heavy on one side or the other
// and if it's out of balance (out of balance is when delta is > 1)
// Since we need to set two values they must be passed in by reference.
void isHeavy(node_t *ptr, int *heavySide, int *outOfBalance) {
  int leftNumNodes, rightNumNodes, tempNum;
  leftRightNumNodes(ptr, &leftNumNodes, &rightNumNodes);
  tempNum = leftNumNodes - rightNumNodes;
  if (tempNum >= 1) {
    *heavySide = LEFT;
    *outOfBalance = (tempNum > 1) ? LEFT : INBALANCE;
  }
  else
    if (tempNum <= -1) {
      *heavySide = RIGHT;
      *outOfBalance = (tempNum < -1) ? RIGHT : INBALANCE;
    }
    else {
      *heavySide = *outOfBalance = INBALANCE;
    }
  return;
}


// Get depth of left and right nodes (they're the args passed in)
int leftRightNumNodes(node_t *ptr, int *leftNum, int *rightNum) {
  if (ptr == NULL) {
    leftNum = rightNum = 0;
  }
  else {
    *leftNum  = depth(ptr->leftNode);
    *rightNum = depth(ptr->rightNode);
  }
}


// Returns 1 if this node is out of balance and no children nodes are
//         0 if this node and no lower nodes are out of balance
//        -1 if there are lower nodes out of balance
int lowestNodeOutOfBalance(node_t *ptr) {
  int temp = 0;
  if (getBalanceValue(ptr) != INBALANCE) {
    if (allChildrenInBalance(ptr) == 1) {
      temp = 1;
    }
    else {
      temp = -1;
    }
  }
}


// Simple routine to get max of two ints
int max(int a, int b) {
  return (a > b) ? a : b;
}


// Return number of nodes in the tree from the node in
int numNodes(node_t *ptr) {
  if (ptr == NULL) {
    return 0;
  }
  else {
    return (1 + numNodes(ptr->leftNode) + numNodes(ptr->rightNode));
  }
}


// Routine to print the tree in order, it shows the path taken to get to the node
void printList(char *hdr, node_t *ptr) {
  if (ptr != NULL) {
    printList("Path leftNode", ptr->leftNode);
    printf("%s: Node address is: %p  value: %d\n",hdr,ptr,ptr->value);
    printList("Path rightNode ",ptr->rightNode);
  }
}


// Print node attributes, passed by ref so we can see real address where ptr is stored
void printNode(char *msg, node_t **ptr) {
  if (*ptr == NULL) {
    printf("%s  &ptr: %p, ptr: %p\n",msg,ptr,*ptr);
  }
  else {
    // Show as much info as possible... that's why multiple printf's
    printf("%s  &ptr: %p, ptr: %p, value: %d, parentNode: %p",msg,ptr,*ptr,(*ptr)->value,(*ptr)->parentNode);
    if ((*ptr)->parentNode != NULL) printf(" (%d)",(*ptr)->parentNode->value);

    printf(", leftNode: %p",(*ptr)->leftNode);
    if ((*ptr)->leftNode != NULL) printf(" (%d)",(*ptr)->leftNode->value);

    printf(", rightNode: %p",(*ptr)->leftNode);
    if ((*ptr)->rightNode != NULL) printf(" (%d)",(*ptr)->rightNode->value);

    printf("\n");
  }
}


// Call printNode for each node in the tree
void printNodeTree(node_t **ptr) {
  if (*ptr != NULL) {
    printNode("printNodeTree:",ptr);
    
    printNodeTree(&((*ptr)->leftNode));
    printNodeTree(&((*ptr)->rightNode));
  }
}


// Print out the tree
void printTree(int level, node_t *ptr) {
  if (ptr == NULL) {
    // printf("Level: %d  ptr is NULL\n",level);    Commented out... added too much noise on console
  }
  else {
    printf("Lvl: %d  dpth: %d  ptr: %p (%d) par: %p (%s) ",level,depth(ptr),ptr,ptr->value,ptr->parentNode,getBalanceStringValue(ptr));
    if (ptr->leftNode == NULL) {
      printf("leftNode is NULL");
    }
    else {
      printf("leftNode: %p (%d)",ptr->leftNode,ptr->leftNode->value);
    }
    if (ptr->rightNode == NULL) {
      printf(" rightNode is NULL");
    }
    else {
      printf(" rightNode: %p (%d)",ptr->rightNode,ptr->rightNode->value);
    }
    printf("\n");
    printTree(level+1,ptr->leftNode);
    printTree(level+1,ptr->rightNode);
  }
}


// Balance node, remember GNPtr is Grandparent of the node that caused the violation, PNPtr is
// it's parent, this routine is passed in the Grand parent node
void rebalanceNode(node_t *GNPtr) {
  int balanceValue = getBalanceValue(GNPtr);
  node_t *PNPtr, *CNPtr, **GGNPtrNode;

  if (DEBUG) {
    printNode(getBalanceStringValueHelper(balanceValue),&GNPtr);
  }

  // Set GGNPtrNode to be the address of the ptr that needs to be adjusted after the rotation, this is the node
  // that has the address of GNPtr
  GGNPtrNode = NULL;
  if (GNPtr->parentNode != NULL) {
    GGNPtrNode = (GNPtr->parentNode->leftNode == GNPtr) ? &(GNPtr->parentNode->leftNode) : &(GNPtr->parentNode->rightNode); 
  }

  switch (balanceValue) {
    case INBALANCE:
      break;       
    case LEFT:
      // We rotate the node passed in to the right of it's leftNode, the
      // nomenclature is the GNPtr is rotated to right of PNPtr
      PNPtr = GNPtr->leftNode;
  
      // GNPtrs left node is the old PNPtr's right node, reset parent of that also
      GNPtr->leftNode = PNPtr->rightNode;
      if (GNPtr->leftNode != NULL) GNPtr->leftNode->parentNode = GNPtr;

      // The GNPtr moves to right under PNPtr
      PNPtr->rightNode = GNPtr;

      // Set parents, PNPtrs is now it's old left child and it's child now has GNPtr 
      PNPtr->parentNode = GNPtr->parentNode;
      GNPtr->parentNode = PNPtr;

      if (GGNPtrNode != NULL) {
        // The node that used to point to GNPtr now has to point to PNPtr
        *GGNPtrNode = PNPtr;
      }
 
      break;
     
    case RIGHT:
      // GNPtr is rotated to left of PNPtr
      PNPtr = GNPtr->rightNode;
    
      // GNPtrs right node is the old PNPtr's left node
      GNPtr->rightNode = PNPtr->leftNode;
      if (GNPtr->rightNode != NULL) GNPtr->rightNode->parentNode = GNPtr;
    
      // The GNPtr moves to left under PNPtr
      PNPtr->leftNode = GNPtr;
           
      // Set parents, PNPtrs is now it's old left child and it's child now has GNPtr
      PNPtr->parentNode = GNPtr->parentNode;
      GNPtr->parentNode = PNPtr;
    
      if (GGNPtrNode != NULL) { 
        // The node that used to point to GNPtr now has to point to PNPtr
        *GGNPtrNode = PNPtr;
      }
     
      break;

    case LEFTRIGHT:
      // We rotate child left around parent first, that will cause this to become LEFT inbalance
      // so we'll call this routine again :), it'll identify left inbalance and perform that logic
      // (to rotate GNPtr to right of PNPtr)
      PNPtr = GNPtr->leftNode;
      CNPtr = PNPtr->rightNode;

      // Parents right node gets the value from the childs left node
      PNPtr->rightNode = CNPtr->leftNode;
      if (PNPtr->rightNode != NULL) PNPtr->rightNode->parentNode = PNPtr;  // Fix child left node parent 

      // Childs left node points to PNPtr and GNPtr now points to CNPtr
      CNPtr->leftNode  = PNPtr;
      GNPtr->leftNode  = CNPtr;

      // Set parents of PNPtr and CNPtr
      PNPtr->parentNode = CNPtr;
      CNPtr->parentNode = GNPtr;
      
      // Call rebalanceNode again, it'll now trigger the LEFT inbalance logic
      rebalanceNode(GNPtr); 

      break;

    case RIGHTLEFT:
      // Rotate PNPtr right around CNPtr, this will cause a RIGHT inbalance so we'll call this routine again 
      PNPtr = GNPtr->rightNode;
      CNPtr = PNPtr->leftNode;

      // PNPtr's left node gets CNPtrs right node, set parent ptr also
      PNPtr->leftNode  = CNPtr->rightNode;
      if (PNPtr->leftNode != NULL) PNPtr->leftNode->parentNode = PNPtr;

      // CNPtr's right node points to PNPtr and GNPtr's right node now points to CNPtr
      CNPtr->rightNode = PNPtr;
      GNPtr->rightNode = CNPtr;

      // Set parents 
      PNPtr->parentNode = CNPtr;
      CNPtr->parentNode = GNPtr; 
     
      rebalanceNode(GNPtr); // Call balance node again... it'll balance the now RIGHT inbalance on GNPtr
      break; 
  } 
}

// Rebalance up the tree from a given node, this is useful in that once we modify a node we only have to
// rebalance up from there, we don't have to rebalance the entire tree
void rebalanceTreeUpFrom(node_t *nodePtr, node_t **headPtr) {
  if (DEBUG) printf("rebalanceTreeUpFrom nodePtr: %p  *headPtr: %p\n",nodePtr,*headPtr);
  if (nodePtr != NULL) {
    rebalanceNode(nodePtr);
    if (nodePtr->parentNode == NULL) { // If node's parent is null then it's the root node
      *headPtr = nodePtr;
      if (DEBUG) printNode("reset headPtr:",headPtr);
    }
    else 
      rebalanceTreeUpFrom(nodePtr->parentNode, headPtr);
  }
}

// ----------------------------------------------------------------------------------------------
//   M A I N   L I N E   
//
//   Used to demonstrate/test tree
//   Author: S. Duffy  
//   Date:   08/12/2022
// ---------------------------------------------------------------------------------------------- 
int main() {
  node_t *head = NULL;
  node_t *node2Balance;
  int nodeLevel2Balance;

  // Initialize tree... have different flavors you can use

  // Multiple inbalances
  if (1 == 1) {
    insertValue(&head, 45);
    insertValue(&head, 20);
    insertValue(&head, 30);
    insertValue(&head, 55);
    insertValue(&head, 30);
    insertValue(&head, 10);
    insertValue(&head, 35);
    insertValue(&head, 33);
    insertValue(&head, 34);
  }

  // Left/Right inbalance
  if (1 == 0) {
    insertValue(&head, 45);
    insertValue(&head, 50);
    insertValue(&head, 30);
    insertValue(&head, 20);
    insertValue(&head, 35);
    insertValue(&head, 33);
    insertValue(&head, 37);
  }

  // Right/Left inbalance
  if (1 == 0) { 
    insertValue(&head, 45);
    insertValue(&head, 35);
    insertValue(&head, 55);
    insertValue(&head, 65);
    insertValue(&head, 60);
  }

  // Left inbalance
  if (1 == 0) { 
    insertValue(&head, 45);
    insertValue(&head, 35);
    insertValue(&head, 25);
    insertValue(&head, 15);
    insertValue(&head, 55);
  }

  // Right inbalance
  if (1 == 0) { 
    insertValue(&head, 45);
    insertValue(&head, 35);
    insertValue(&head, 55);
    insertValue(&head, 65);
    insertValue(&head, 75);
  }

  // Check the parent pointers of each child 
  printf("\nChecking parent pointers\n");
  checkParentPointers(&head);

  // Call 'printList' to print tree in order
  if (1 == 0) {
    printf("\nprintList\n");
    printList("Start", head);
  }

  // Node Search
  if (1 == 0) {
    printf("\nTest searching\n\n");
    printf("found 30 at %p\n",findNode(head,30));
    printf("found 31 at %p\n\n",findNode(head,31));
  }
  
  // Print tree as it was loaded
  printf("\nprintTree called with 0,head\n");
  printTree(0,head);
  
  printf("\nNumber of nodes in tree: %d\n",numNodes(head));

  // Node balancing
  if (1 == 1) {
    printf("\nNode Balancing\n");
    nodeLevel2Balance = 0;
    node2Balance      = NULL;
    getNodeToBalance(head, 1, &node2Balance, &nodeLevel2Balance);
    printf("Node needing to balance: %p, at level %d\n",node2Balance,nodeLevel2Balance); 
  
    while (node2Balance != NULL) {
      rebalanceNode(node2Balance);
      if (head == node2Balance) {  // If rebalanced head then have it point to it's new parent (it rotated left or right)
        head = node2Balance->parentNode;
      }

      printf("\nTree after rebalance:\n");
      printTree(0,head);
      
      node2Balance = NULL;
      nodeLevel2Balance = 0;
      getNodeToBalance(head, 1, &node2Balance, &nodeLevel2Balance);
    }
  }

  // Node search and find parent/grandparents (with and without smartSearch on (using value to determine path))
  if (1 == 0) {
    // Search for a node, get it's parent and grandparent, this won't use 'smart search' (last arg)
    printf("\nFinding parent/grandparent searching the tree (dumb search)\n");
    node_t *node2Find = findNode(head,30);
    printf("found 30 at %p\n",node2Find);
    printf("it's parent is %p\n",getNodesParent(head,node2Find,0));
    printf("it's grand parent is %p\n",getNodesParent(head,getNodesParent(head,node2Find,0),0));

    printf("Finding parent/grandparent using smartSearch, when there is no grandParent\n");
    node2Find = findNode(head,20);
    printf("found 20 at %p\n",node2Find);
    printf("it's parent is %p\n",getNodesParent(head,node2Find,1));
    printf("it's grand parent is %p\n",getNodesParent(head,getNodesParent(head,node2Find,1),1));

    printf("Finding parent/grandparent compare smartSearch to dumb search\n");
    node2Find = findNode(head,34);

    // This searches for the same value with smartSearch off and then with it on, with the DEBUG on you'll
    // see the number of calls to getNodesParent 
    printf("\nfound 34 at %p\nsmartSearch OFF\n",node2Find);
    printf(" parent: %p\n",getNodesParent(head,node2Find,0));
    printf("smartSearch ON\n");
    printf(" parent: %p\n",getNodesParent(head,node2Find,1));
  }

  // Check the parent pointers of each child 
  printf("\nChecking parent pointers\n");
  checkParentPointers(&head);

  // Test deleting nodes
  if (1 == 1) {
    printf("\nDeleting nodes\n");
    printf("Delete 45\n");
    deleteValue(&head,45);
    printNodeTree(&head);

    printf("Delete 10\n");
    deleteValue(&head,10);
    printNodeTree(&head);

    printf("Delete 34\n");
    deleteValue(&head,34);
    printNodeTree(&head);

    // Check the parent pointers of each child 
    printf("\nChecking parent pointers\n");
    checkParentPointers(&head);
  }
  
  printf("\n\nFinal tree:\n");
  printTree(0,head);
}

