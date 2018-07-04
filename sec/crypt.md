## symmetric/private-key encryption
- the key is used for both encryption and decryption.
- breaking is to find the key

## Kerckhoffs'  principle
only key is relied upon, algrythm should be public.

## sufficient key space principle

## polynomial-time complexity (多项式时间复杂度): 问题规模n不出现在指数上，O(nlog(n)), O(n^3)

## exponential time complexity
- suffers from combinatorial explosion (curse of dimensionality)

## Linear feedback shift register(LFSR)
shift one bit, transformed(xor) with one or more bits in specified position(taps), the result is the output bit, which is pushed in the other direction for next step.
- the output bits form a sequence, which is periodic. 

when taps meet certain criteria, the sequence period (maximum cycle length) can reach the maximum, that is 2^n-1 (n is number of bits in register), called Maximum-Length-Sequence.

the criteria is indicated in special Primitive Polynomial. maybe more than one.
one for 8 bits: x^8+x^6+x^5+x^4+x^0
tap position is 6,5,4



## perfect secracy
information-theoretic security, no enough information to succeed
- key space should not be less than message space
- key can be used only once

## non-deterministic encryption scheme
- same input, different output
- is dedicated to overcome CPT

## one-time pad is perfect encryption
- apply a keystream with the same length of plain text to encode and decode via XOR.  
- the keystream can not be used any longer
- it's encrypted one unit by one unit

**it's the essential algorithm for stream cypher encryption.**

## computational security
to overcome the impracticability that perfect security requires key has the same length with plain text
- to break needs long enough computational time (e.g. secure in effecient time, that is the time is a polynomial of n)
- the probability to break is quite small (less than the inverse of the above polynomial)

## brute force search
- exponential time complex (for bits, key space is 2^n)
- theoritically can break every encryption

## pseudorandom generator
- input should be random


## pseudorandom functions
- input can be arbitrary (even the same input gets random output)
- a key selects one function (number of `2^n` functions are selected from true random space `2^(n*2^n)`)
- the true random functions space is `2^(n*2^n)`: the function doesn't do permutation, e.g. **different input may result the same output**, so all output of each input form a single large number, size is `n*2^n`

one sample function:
```
the large number corresponds to a function (length is n*2^n)
                     |
                 ---------
0 0 0     ->    |  0 0 0  |
0 0 1     ->    |  0 0 0  |
0 1 0     ->    |  0 0 0  |
0 1 1     ->    |  0 0 0  |
1 0 0     ->    |  0 0 0  |
1 0 1     ->    |  0 0 0  |
1 1 0     ->    |  0 0 0  |
1 1 1     ->    |  0 0 0  |
                 ---------
```
## pseudorandom permutation
a pseudorandom function which does permutation (input and output are one-to-one)

* one-to-one is necessary for decryption.

## strong pseudorandom permutation
when inverse permutation is accessible the function still seems random.

## stream cipher
key stream generating algorythm, adopted by stream encryption
- LFSR
- rc4

## stream encryption
- based on one-time pad, keystream is pseudorandom, generated from a short key
 via stream cipher
- suitable for variable length stream
- encrypting units donot depend on each other. 
- keystream should not be used twice
 * sync mode, quite long stream shared beforehand, of which a part is used each time
 * async mode, a random stream is generated each time, being used to generate keystream along with shared key, is sent as part of cypher text

## block cipher 
* its pseudorandom permutation (better strong)
* the sequence of blocks form a permutation of numbers of 2^n, each time an order is outputed, then the space is `2^n!`
