/**
* The MIT License (MIT)
*
* Copyright (c) 2019-2020 Vincent Davis Jr.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#define LUCUR_CLOCK_API
#include <lucom.h>

/* Taken From: https://www.roxlu.com/2014/047/high-resolution-timer-function-in-c-c-- */
uint64_t wlu_hrnst() {
  static uint64_t is_init = 0;
  static struct timespec linux_rate;

  if (is_init == 0) {
    clock_getres(CLOCKID, &linux_rate);
    is_init = 1;
  }

  uint64_t now;
  struct timespec spec;
  clock_gettime(CLOCKID, &spec);
  now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
  return now;
}

/**
* Gratuitous Function Usage example:
* dest = 36, src = 9, bits = 4, do = destination original
* dest = 0010 0100 src = 0000 1001 (original)
* 1) dest = 0000 0000 src = 0000 1001 (after: dest_t ^ dest_t)
* 2) dest = 0000 0000 src = 0000 1001 (after: src_t & bitmask)
* 3) dest = 0000 1001 src = 0000 1001 (after: dest_t | src_t)
* 4) dest = 0000 1001 do  = 0010 0000 (after: dest_orig & ~bitmask) 0010 0101 & 1111 0000 = 0010 0000
* 5) dest = 0010 1001 do  = 0010 0000 (after: dest_t | dest_orig)
* Output = 41
*/
void *wlu_bitcpy(void *dest, void *src, size_t bits) {
  /**
  * If there are no bits to copy, or no addresses available
  * there is no need to execute function
  */
  if (!dest || !src || bits == 0) return NULL;

  char *dest_t = (char *) dest;
  char *src_t = (char *) src;

  /* Store original value */
  char dest_orig = *dest_t;
  char src_orig = *src_t;

  /**
  * Create bitmask to obtain only the bits that need to be copy
  * This is saying take the number one (power of two) as original value
  * 00000001
  * Bit shift 1 left x amount of times (x = 3)
  * 00001000 = 8
  * Minus answer by one
  * 00000111 = 7
  * This will allow for one to get the lower half subset of bits required to copy
  *
  * ~bitmask
  * This does the same thing but instead gets the upper half subset of bits required to copy
  * 0000 0111 = 1111 1000 (bits you want to keep)
  */
  int bitmask = (1 << bits) - 1;

  /**
  * Since bits do not need to be binary compatible:
  * 1. zero out destination bits by XOR'ing bits with itself
  * 2. AND bitmask with src. Thus setting only bits that ones needed for copy operation
  * 3. OR the resulting src bits with the destination bits thus setting its bit values
  * 4. AND complemented bitmask with dest_orig. Thus setting only the bits that need to be copied back to dest
  * 5. OR the resulting dest_orig bits with dest
  */
  *dest_t ^= *dest_t;
  *dest_t |= (*src_t & bitmask);
  *dest_t |= (dest_orig & ~bitmask);

  /* Reset src original */
  *src_t = src_orig;

  return dest;
}
