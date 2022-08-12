#include <stdio.h>

// This takes in a byte of data and returns a bit 
// representation of it.
// Logic: have byte where you turn on each bit position individually,
//        you 'AND' this with the value being tested, if the result is
//        not zero then you know that bit is on in the value.  You
//        set the appropriate value in the string being returned.
//        Note: It's 'static' so that it's preserved after the function
//              ends, also the reason you need to reset it to '0' (to 
//              prevent previous values leaving bit on).
char *byteToBits(unsigned char theByte) {
  static unsigned char rtnBits[9] = "00000000";
  unsigned char mask = 1;
  char idx;
  for (idx = 7; idx >= 0; idx--) {
    if ((theByte & mask) > 0) {
      rtnBits[idx] = '1';
    }
    else {
      rtnBits[idx] = '0';
    }
    mask<<=1; // shift bit to next higher position
  }
  return rtnBits;
}

// ---------------------------------------------------------------
int main(int argc, char *argv[]) {

  printf("\nPowers: 2^0 to 2^7\n");
  unsigned char theOne = 1;
  char idx;
  for (idx = 0; idx < 8; idx++) { 
    printf("2^%d: %d\n",idx,theOne);
    theOne<<=1;
  }
 
  // Show binary values from 0 -> 255
  if (1 == 1) { 
    unsigned char i, j, k;
    i = 0;
    printf("\nByte representations 0 -> 255\n");
    for (j = 0; j < 32; j++) {
      for (k = 0; k < 8; k++) {
        printf("%s ",byteToBits(i++));
      }
      printf("\n");
    }
  } 
  
}
