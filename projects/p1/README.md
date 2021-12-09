# Everybit (Bit Hacks)
Fast bit rotator. Rotates a string of bits within a bit array by a specified
amount to the left or right. 

## Methods
Instead of rotating the bits one-by-one to the left or right to manipulate the
bit array, we use more efficient strategies. In particular,

|    Method   |        Pros       |           Cons          |
|:-----------:|:-----------------:|:-----------------------:|
| AB Rotation | Easy to implement |      Scales poorly      |
|    Cyclic   |   Constant space  | Scattered memory access |
|   Reversal  |  Constant storage |            --           |

### AB Rotation
The most obvious way to perform a circular left rotation is to consider the
string to be rotated to be of the form `ab`, where `a` and `b` are bit strings.
We wish to transform `ab` to `ba`. The simplest method is to copy `a` to an
auxiliary array, move `b` to its final location, then copy `a` from the
auxiliary array to its final location. This method is simple, but the need for a
large auxiliary array can be problematic for cache performance when rotating
long strings.

### Cyclic
To minimize the number of bit movements, a cyclic approach can be implemented,
where each bit moves ahead by the specified amount, modulo the length of the
region to be rotated, eventually looping back to the first bit in the cycle.
This strategy places all bits in the correct locations while using a constant
amount of auxiliary space, but memory accesses are scattered, which can
adversely impact caching.

### Reversal
Finally, there is a clever approach that moves every bit twice without using
auxiliary memory. Again treating the string to be rotated as `ab`, observe the
identity `(a^R b^R)^R = ba`, where `R` is the operation that reverses a string.
The “reverse” operation can be accomplished using only constant storage. Thus,
with 3 reversals of bit strings, the string can be rotated.

## Tests
We have added a test suite that runs through everybit's API and ensures all
functions are working as expected. These tests are accessible in
data/mytests. The command `make testquiet` will build the test program and
run the unit (correctness) and performance (speed) tests.

## TODOs
- [ ] Remove makefile; ported to CMake
- [ ] Remove `sample_test_a` function; just for quickly print testing bitarray
- [ ] Add unit tests for the other functions in the public API (bitarray)
