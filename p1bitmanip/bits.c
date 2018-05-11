/* 
 * CS:APP Data Lab 
 * 
 * Omar Salemohamed salemohamedo
 * 
 * bits.c - Source file with your solutions to the project.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the project by
editing the collection of functions in this source file.

CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
4 The maximum number of ops for each function is given in the
     header comment for each function.

/*
 * STEP 2: Modify the following functions according to the coding rules.
 * 
 */


#endif
/* 
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */

int absVal(int x) {
  int sign = (x>>31)&1; //finds the sign of x
  int posorneg = x^(x>>31); //converts negative x to positive x - 1 or leaves positive x as x
  return sign + posorneg; //if x is positive then 0 will be added, if x is negative 1 will be added
}
/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
  //overflow occurs when the sum has a different sign of x and y (and they have the same sign)
  int signx = (x>>31)&0x1;
  int signy = (y>>31)&0x1;
  int sumsign = ((x + y)>>31)&0x1;
  //int xandysame = !(signx^signy); //1 if x and y are same sign else 0
  return ((!(sumsign^signy))|(!(sumsign^signx)));
}
/* 
 * allEvenBits - return 1 if all even-numbered bits in word set to 1
 *   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allEvenBits(int x) {
  int bitmask = (0x55<<24)|(0x55<<16)|(0x55<<8)|(0x55); //mask consisting of only even bits set to 1
  int evenbits = bitmask&x; //extract only evenbits
  int checkifallset = evenbits^bitmask; //if all even bits are set, this should be 0 
  return(!checkifallset);
}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  //if you OR  the sign of x and -x you will get all 1's if the x is non-zero or all 0's if x is 0.
  int signx = (x>>31);
  int signnegx = (~x + 1)>>31;
  int or = signx|signnegx; 
  return or + 1;
}
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
  return ~(~x|~y);
}
/*
 * bitMask - Generate a mask consisting of all 1's 
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
  int diff = highbit + (~lowbit + 1); //find the difference of highbit - lowbit
  int signdiff = ~(diff>>31); //all ones if highbit - lowbit >= 0 else all 0's

  int allone = ~0;
  int highmask = allone << highbit;
  int lowmask = allone << lowbit;
  int high_equal_low = lowmask ^ highmask; //should equal 0 if highbit = lowbit
  int bitmask = high_equal_low|(1<<highbit); //should contain one 1 if highbit=lowbit, or the correct bitmask
  return bitmask&signdiff; //will return 0 if lowbit > highbit
}
/*
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  return (~(x&y))&(~(~x&~y));
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
    int nbits = n<<3;
    int mbits = m<<3;
    int nbyte = (x>>nbits)&0xff;
    int mbyte = (x>>mbits)&0xff;
    int swapped = (nbyte<<mbits)|(mbyte<<nbits);
    int zeromask = ~((0xff<<nbits)|(0xff<<mbits));
    return (x&zeromask)|swapped;
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  //first evaluate if x is true or false (0)
  int boolx = !!x;
  //create mask of 0's if false and 1's if true
  int maskx = (boolx<<31)>>31;
  //if x is true, almosty will contain a version of y, with 1's missing in indices that were also set in z
  int almosty = maskx&(y ^ z);
  //if almosty is 0 (x is false) then this will return z, or else it will fill in the indices of y where 0's should be 1's
  int returnones = almosty ^ z;
  return returnones;
}
/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x) {
  return ((x&0x1)<<31)>>31;
}
/*
 * ezThreeFourths - multiplies by 3/4 rounding toward 0,
 *   Should exactly duplicate effect of C expression (x*3/4),
 *   including overflow behavior.
 *   Examples: ezThreeFourths(11) = 8
 *             ezThreeFourths(-9) = -6
 *             ezThreeFourths(1073741824) = -268435456 (overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */
int ezThreeFourths(int x) {
  //x multiply by 3/4 can be thought of as (x+x+x)>>2
  int add3 = x + x + x;
  int div4 = add3>>2;
  int signadd = (add3>>31)&0x1;
  int remainderadd3 = add3&0x3;
  int rem_and_pos = signadd & !!remainderadd3;
  return div4 + rem_and_pos;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  int size_minus_n = 32 + (~n + 1);
  return !(((x<<size_minus_n)>>size_minus_n)^x);
}
/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
  return (x>>(n<<3))&0xFF;
}
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */


int isAsciiDigit(int x) {
  int TMIN = 0x1<<31;
  int lower = ~(0x30);
  int upper = ~(TMIN|0x39);
  //if x plus lower or x + upper bound is negative than x is out of range
  int addlower = x+lower+1;
  int addupper = x+upper;
  return !(((addlower&~0)>>31)|((addupper&~0)>>31));
}
/* 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
  return !(x^y);
}
/*
 * isLess - if x < y  then return 1, else return 0 
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLess(int x, int y) {
  //x can be less than y is both have same sign and x - y < 0
  //or if x is less than 0 and y is 0 or greater
  int signx = (x>>31)&0x1;
  int signy = (y>>31)&0x1;

  int signequal = !(signy^signx);

  int nonnegative = ((x + ~y)>>31)&0x1;

  int xyequal = !!(y^x);

  int temp = nonnegative&xyequal;

  return (signx&(!signy))|(temp&signequal);



}
/*
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
  return !(x>>31);
}
/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int isPower2(int x) {
  //take advantage of fact that neg can't be power of two
  //and if x is pow 2, then x-1 should be all 1's i.e 8 = 01000 7 = 00111
  int xminus1 = x + ~0;
  int signx = (x>>31)&0x1;
  int x_is_0 = !!x;
  //one minus x & x should be 0 if x is a power of 2 or if x is zero, so the x_is_0 check is added
  //and the signx check is added to see if x is negative
  return (!(xminus1&x))&!signx&x_is_0;


}
/*
 * isTmin - returns 1 if x is the minimum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmin(int x) {
  return !((x^(~x+1))+ !x);
}
/*
 * minusOne - return a value of -1 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int minusOne(void) {
  return ((0x1<<31)>>31);
}
/*
 * rotateLeft - Rotate x to the left by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateLeft(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateLeft(int x, int n) {
  int diff = 32 + ~n;
  int neg1 = ~0;
  int rotate = x << n;
  int zeromask = ~(neg1<<n);
  int shift = (x>>(diff+1));
  int shifted = shift&zeromask;
  return rotate|shifted;
}
/*
 * satMul2 - multiplies by 2, saturating to Tmin or Tmax if overflow
 *   Examples: satMul2(0x30000000) = 0x60000000
 *             satMul2(0x40000000) = 0x7FFFFFFF (saturate to TMax)
 *             satMul2(0x60000000) = 0x80000000 (saturate to TMin)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int satMul2(int x) {
  int TMIN = 0x1<<31;

  int times2 = x+x;
  int signmaskx = (x>>31);

  int change = (times2^x)>>31;

  int posor0 = ~signmaskx^TMIN;

  return (posor0&change)|(~change & times2);

}
/*
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {
  //overflow can be determined by two conditions: x and y have different signs, or the difference and
  //y have different signs

  //finding signs of x,y, and x-y
  int signx = (x>>31)&0x1;
  int signy = (y>>31)&0x1;
  int signdiff = ((x + (~y +1))>>31)&0x1;
  int xandysigns = signx ^ signy;
  int diffandysigns = !(signdiff^signy);
  return !(diffandysigns&xandysigns);
}

