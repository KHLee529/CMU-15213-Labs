# Data Lab Solution

###### 

## Questions

### integers

#### bitwise representation

- unsigned numbers

`0xFFFFFFFF = Tmax`
`0x00000000 = 0`

- 2's complment numbers

`0xFFFFFFFF = -1`
`0x80000000 = Tmin`
`0x7FFFFFFF = Tmax`
`0x00000000 = 0`

#### `int bitXor(int x, int y)`

`x^y` using only `~` and `&` 

Limits
- Legal ops: ~ &
- Max ops: 14

```
Truth Table
| x | y |x^y|
| 0 | 0 | 0 |
| 0 | 1 | 1 |
| 1 | 0 | 1 |
| 1 | 1 | 0 |
```

`xor` can be represented as `(~x & y) | (x & ~y)`
by applying boolean algebra as shown below
```
  (~x & y) | (x & ~y)
= ((~x & y) | x) & ((~x & y) | ~y)
= ((~x | x) & (y | x)) & ((~x | ~y) & (y | ~y))
= (1 & (y | x)) & ((~x | ~y) & 1)
= (y | x) & (~x | ~y)
= ~(~x & ~y) & ~(x & y)
```
it becomes `~(~x & ~y) & ~(x & y)`

#### `int tmin(void)`

return minimum two's complement integer 

Limits
- Legal ops: ! ~ & ^ | + << >>
- Max ops: 4

By definition, `Tmin` is represented with leading bit `1` and following bits `0`

#### `int isTmax(int x)`

returns 1 if x is the maximum, two's complement number, and 0 otherwise 

Limits
- Legal ops: ! ~ & ^ | +
- Max ops: 10

`Tmax` has property `~(Tmax + 1) == Tmax`
by replace `x == y` operator with `!(x ^ y)`

Notice that `Tmin` also has this property so it's necessary to take `Tmin` as exception

#### `int allOddBits(int x)`

return 1 if all odd-numbered bits in word set to 1
where bits are numbered from 0 (least significant) to 31 (most significant)

Examples 
- allOddBits(0xFFFFFFFD) = 0
- allOddBits(0xAAAAAAAA) = 1

Limits
- Legal ops: ! ~ & ^ | + << >>
- Max ops: 12

Easily check if x equals to the number which is only composed of odd bits of x

#### `int negate(int x)`

return -x 

Example
- negate(1) = -1.

Limits
- Legal ops: ! ~ & ^ | + << >>
- Max ops: 5

by definition -x = ~x + 1
Notice that `Tmin` is an exception that `~Tmin + 1 = Tmin`

#### `int isAsciiDigit(int x)`

- return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')

Example
- isAsciiDigit(0x35) = 1.
- isAsciiDigit(0x3a) = 0.
- isAsciiDigit(0x05) = 0.

Limits
- Legal ops: ! ~ & ^ | + << >>
- Max ops: 15

```
bitwise representation
0x30 = 0b00110000
0x31 = 0b00110001
0x32 = 0b00110010
0x33 = 0b00110011
0x34 = 0b00110100
0x35 = 0b00110101
0x36 = 0b00110110
0x37 = 0b00110111
0x38 = 0b00111000
0x39 = 0b00111001
```

By observing the bitwise representation of those numbers, we can find that the check way is 

`((x >> 4) == 3) && ((x >> 3) == 6 || (x >> 1) == 0x1c)`

#### `int conditional(int x, int y, int z)`

same as x ? y : z 

Example
- conditional(2,4,5) = 4

Limits
- Legal ops: ! ~ & ^ | + << >>
- Max ops: 16

we can set x to `0xffffffff` if x do not equal to 0, else set x to 0, 
then easily return `(x & y) | (~x ^ z)`


#### `int isLessOrEqual(int x, int y)`

if x <= y  then return 1, else return 0 

Example
- isLessOrEqual(4,5) = 1.

Limits
- Legal ops: ! ~ & ^ | + << >>
- Max ops: 24

To get whether x is less or equal to y, we can check whether x - y is less or equal to 0.
Besides, the way to calculate `-` with the legal operators is to use the result of last question.
Then the question becomes 2 parts.
1. check if x is equal to y by !(x ^ y)
2. if not check if x - y is less than 0
However, if x < 0 and y > 0, x - y may cause overflow when using `x - y = x + (~y + 1)`
and possibly make x - y > 0 which is obviously not expected,
it's not a good way to naively check if x + (~y + 1) < 0.
Therefore I seperate this part again into 2 parts.
1. if x, y are opposite signs, easily check if x < 0.
2. if x, y are the same sign, take off the sign bit and check if x - y < 0.
by this way, we can get rid of overflow and `-Tmin = Tmin` issue.

```
Procedure flow
            (true)  +-- return true 
                    |
-  x == y ? --------+                      (true)  +-- return (~Tmin & x) - (~Tmin & y) < 0?
                    |                              |
            (false) +-- sign(x) == sign(y)? -------+ 
                                                   |
                                           (false) +-- return x < 0?
```

#### `int logicalNeg(int x)`

implement the ! operator, using all of the legal operators except !

Examples
- logicalNeg(3) = 0
- logicalNeg(0) = 1

Limits
- Legal ops: ~ & ^ | + << >>
- Max ops: 12

I got this by easily `or` all the bits to the right most bit then return that one.
Concretely, use binary seperation, first `or` the left 16 bits to the right 16 bits,
then `or` the left 8 bits to the left 8 bits inside the right 16 bits, etc.

#### `int howManyBits(int x)`

return the minimum number of bits required to represent x in two's complement

Examples
- howManyBits(12) = 5 
- howManyBits(298) = 10 
- howManyBits(-5) = 4 
- howManyBits(0)  = 1 
- howManyBits(-1) = 1 
- howManyBits(0x80000000) = 32 

Limits
- Legal ops: ! ~ & ^ | + << >>
- Max ops: 90

Strategy:
 fine the first bit which diff from the sign bit and add 1
 search algorithm: Binary search

### floats

float bitmaps
```
0    00000000 00000000000000000000000
sign Ebits    Mbits
```
sign: 0: +, 1: -
f = M * 2 ^ E
M = 0.Mbits if EBits = 0
M = 1.Mbits if 0 < EBits < 0xfe
E = Ebits - 2^8 (E = 0 when Ebits = 0x7f)
f = `+-inf` if Ebits = 0xff and Mbits = 0
f = `NaN` if Ebits = 0xff and Mbits != 0

#### `unsigned floatScale2(unsigned uf)`

Return bit-level equivalent of expression 2*f for floating point argument f.
Both the argument and result are passed as unsigned int's, but
they are to be interpreted as the bit-level representation of
single-precision floating point values.
When argument is NaN, return argument

Limits
- Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
- Max ops: 30

By the floating number definition, we can know that 
if Ebits = 0, just left shift x by one bit and assign the origin sign bit
else if Ebits != 0, just increase E value by one

#### `int floatFloat2Int(unsigned uf)`

Return bit-level equivalent of expression (int) f for floating point argument f.

Argument is passed as unsigned int, but it is to be interpreted as the bit-level representation of a
single-precision floating point value.
Anything out of range (including NaN and infinity) should return 0x80000000u.

Limits
- Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
- Max ops: 30

By the floating number definition, we can know that 
if E value < 0 (int) f = 0
else (int) f = Mbits left shifts by N value bits

#### `unsigned floatPower2(int x)`

Return bit-level equivalent of the expression 2.0^x
(2.0 raised to the power x) for any 32-bit integer x.

The unsigned value that is returned should have the identical bit
representation as the single-precision floating-point number 2.0^x.
If the result is too small to be represented as a denorm, return
0. If too large, return +INF.

Limits
- Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
- Max ops: 30 

float can only represent number between 2^-150 < f < 2^127, so if x is not in the range (-150, 127),
return 0 or INF.
Otherwise, return f by the definition of floating number