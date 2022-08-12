# C Playground

## This has some sample C code that I was playing around with to demonstrate different algorithms
- linkedList.c
  - Simple linked list

- linkedList2.c
  - A little more sophisticated liked list

- playArea.c
  - Play area, has a little function to show the binary representation of a byte

- pointersAndMore.c
  - Mainly pointer stuff but has another implmentation of byte -> bitString conversion

- tree.c 
  - fyi: Functions are place alphabetically in the code (easier to find that way)
  - The code shows how to implement a tree along with various utility functions.  This tree includes has a 'parent pointer' that points back to a nodes parent.  fyi: I included a routine to find a nodes parent; if you don't want a 'parent pointer' in the node just replace references in the code (parentNode) to use 'getNodesParent'.  
  - There's logic to balance the tree; it can balance during tree updates or you can balance the tree after it's built.  The value of REALTIMEBALANCE will dictate if balancing happens during updates.