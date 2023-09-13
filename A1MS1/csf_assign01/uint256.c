#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;
  result.data[0] = val;
  for (int i = 1; i < 8; i++) {
    result.data[i] = 0;
  }
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 7 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;
  for (int i = 0; i < 8; i++) {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result = uint256_create_from_u32(0);

  int hex_len = strlen(hex);
  if (hex_len > 64) {
    hex += hex_len - 64;
    hex_len = 64;
  }
  
  uint32_t chunk = 0;
  int curr_index = hex_len - 1;

  for (int i = 0; i < 8; i++) {
      int chunk_size = 0;
      if (curr_index >= 7) {
        chunk_size = 8;
      }
      else {
        chunk_size = curr_index + 1;
      }
      char hex_sub[9];
      strncpy(hex_sub, hex + curr_index - chunk_size + 1, chunk_size);
      
      hex_sub[chunk_size] = '\0';
      chunk = strtoul(hex_sub, NULL, 16);
      result.data[i] = chunk;
      curr_index -= chunk_size;
      if (curr_index < 0) {
	break;
      }
  }
	 
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = (char *)malloc(65);
  char *hex_chunk = hex;
  uint32_t current;
  int curr_hex = 0;
  int zero_flag = 1;
  
  for (int i = 7; i >= 0; i--) {
    if (val.data[i]) {
      current = val.data[i];
      if (zero_flag) {
	curr_hex = sprintf(hex_chunk, "%x", current);
	zero_flag = 0;
      }
      else {
	curr_hex = sprintf(hex_chunk, "%08x", current);
      }
      hex_chunk += curr_hex;
    }
    else {
      if (!(hex_chunk == hex)) {
	current = val.data[i];
	curr_hex = sprintf(hex_chunk, "%08x", current);
	hex_chunk += curr_hex;
      }
    }
  }

  if (hex_chunk == hex) {
    *hex_chunk++ = '0';
  }

  *hex_chunk = '\0';
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 3 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  int overflow = 0;

  for (int i = 0; i < 8; i++) {
    sum.data[i] = left.data[i] + right.data[i] + overflow;
    if (sum.data[i] < left.data[i] || sum.data[i] < right.data[i]) {
      overflow = 1;
    }
    else {
      overflow = 0;
    }
  }
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  right = uint256_negate(right);
  result = uint256_add(left, right);
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  int overflow = 1;
  uint32_t inverse;
  
  for (int i = 0; i < 8; i++) {
    inverse = ~val.data[i];
    result.data[i] = inverse + overflow;
    if (result.data[i] < inverse) {
      overflow = 1;
    }
    else {
      overflow = 0;
    }
  }

  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result;

  char *bin = (char *)malloc(257);
  char *val_chunk = bin;

  int curr_val = 0;
  for (int i = 7; i >= 0; i--) {
    //if (val.data[i]) {
      uint32_t current = val.data[i];
      curr_val = sprintf(val_chunk, "%08b", current);

      val_chunk += curr_val;

      //}
  }
  if (val_chunk == bin) {
    *val_chunk++ = '0';
  }
  *val_chunk = '\0';

  int bin_len = strlen(bin);

  char sub_bin[nbits + 1];

  strncpy(sub_bin, bin, nbits);
  sub_bin[nbits] = '\0';

  memmove(bin, bin + nbits, bin_len - nbits + 1);
  strcat(bin, sub_bin);

  for (int i = 7; i >= 0; i--) {
  }
  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}
