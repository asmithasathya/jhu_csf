#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero; // the value equal to 0
  UInt256 one;  // the value equal to 1
  UInt256 max;  // the value equal to (2^256)-1
  UInt256 msb_set; // the value equal to 2^255
  UInt256 rot; // value used to test rotations
} TestObjs;

// Helper functions for implementing tests
void set_all(UInt256 *val, uint32_t wordval);

#define ASSERT_SAME(expected, actual) \
do { \
  ASSERT(expected.data[0] == actual.data[0]); \
  ASSERT(expected.data[1] == actual.data[1]); \
  ASSERT(expected.data[2] == actual.data[2]); \
  ASSERT(expected.data[3] == actual.data[3]); \
  ASSERT(expected.data[4] == actual.data[4]); \
  ASSERT(expected.data[5] == actual.data[5]); \
  ASSERT(expected.data[6] == actual.data[6]); \
  ASSERT(expected.data[7] == actual.data[7]); \
} while (0)

#define INIT_FROM_ARR(val, arr) \
do { \
  for (unsigned i = 0; i < 8; ++i) \
    val.data[i] = arr[i]; \
} while (0)

// Functions to create and cleanup the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Declarations of test functions
void test_get_bits(TestObjs *objs);
void test_create_from_u32(TestObjs *objs);
void test_create(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_rotate_left(TestObjs *objs);
void test_rotate_right(TestObjs *objs);

int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_get_bits);
  TEST(test_create_from_u32);
  TEST(test_create);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_negate);
  TEST(test_rotate_left);
  TEST(test_rotate_right);

  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all(UInt256 *val, uint32_t wordval) {
  for (unsigned i = 0; i < 8; ++i) {
    val->data[i] = wordval;
  }
}

TestObjs *setup(void) {
  TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));

  // initialize several UInt256 values "manually"
  set_all(&objs->zero, 0);
  set_all(&objs->one, 0);
  objs->one.data[0] = 1U;
  set_all(&objs->max, 0xFFFFFFFFU);

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
  INIT_FROM_ARR(objs->msb_set, msb_set_data);

  // value with nonzero values in least significant and most significant words
  // Note that the hex representation of this value is
  //   CD000000 00000000 00000000 00000000 00000000 00000000 00000000 000000AB
  uint32_t rot_data[8] = { 0x000000ABU, 0U, 0U, 0U, 0U, 0U, 0U, 0xCD000000U };
  INIT_FROM_ARR(objs->rot, rot_data);

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_get_bits(TestObjs *objs) {
  ASSERT(0U == uint256_get_bits(objs->zero, 0));
  ASSERT(0U == uint256_get_bits(objs->zero, 1));
  ASSERT(0U == uint256_get_bits(objs->zero, 2));
  ASSERT(0U == uint256_get_bits(objs->zero, 3));
  ASSERT(0U == uint256_get_bits(objs->zero, 4));
  ASSERT(0U == uint256_get_bits(objs->zero, 5));
  ASSERT(0U == uint256_get_bits(objs->zero, 6));
  ASSERT(0U == uint256_get_bits(objs->zero, 7));

  ASSERT(1U == uint256_get_bits(objs->one, 0));
  ASSERT(0U == uint256_get_bits(objs->one, 1));
  ASSERT(0U == uint256_get_bits(objs->one, 2));
  ASSERT(0U == uint256_get_bits(objs->one, 3));
  ASSERT(0U == uint256_get_bits(objs->one, 4));
  ASSERT(0U == uint256_get_bits(objs->one, 5));
  ASSERT(0U == uint256_get_bits(objs->one, 6));
  ASSERT(0U == uint256_get_bits(objs->one, 7));

  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 0));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 1));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 2));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 3));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 4));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 5));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 6));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 7));
}

void test_create_from_u32(TestObjs *objs) {
  UInt256 zero = uint256_create_from_u32(0U);
  UInt256 one = uint256_create_from_u32(1U);

  ASSERT_SAME(objs->zero, zero);
  ASSERT_SAME(objs->one, one);

  UInt256 result = uint256_create_from_u32(0xABCDEFABU);
  ASSERT(0xABCDEFABU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);
}

void test_create(TestObjs *objs) {
  (void) objs;

  uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
  UInt256 val1 = uint256_create(data1);
  ASSERT(1U == val1.data[0]);
  ASSERT(2U == val1.data[1]);
  ASSERT(3U == val1.data[2]);
  ASSERT(4U == val1.data[3]);
  ASSERT(5U == val1.data[4]);
  ASSERT(6U == val1.data[5]);
  ASSERT(7U == val1.data[6]);
  ASSERT(8U == val1.data[7]);
}

void test_create_from_hex(TestObjs *objs) {
  UInt256 zero = uint256_create_from_hex("0");
  ASSERT_SAME(objs->zero, zero);

  UInt256 multzero = uint256_create_from_hex("000");
  ASSERT_SAME(objs->zero, multzero);

  UInt256 twoIndexZero = uint256_create_from_hex("0000000000");
  ASSERT_SAME(objs->zero, twoIndexZero);
    
  UInt256 one = uint256_create_from_hex("1");
  ASSERT_SAME(objs->one, one);

  UInt256 a = uint256_create_from_hex("a");
  ASSERT(10 == a.data[0]);
  ASSERT(0 == a.data[1]);
  ASSERT(0 == a.data[2]);
  ASSERT(0 == a.data[3]);
  ASSERT(0 == a.data[4]);
  ASSERT(0 == a.data[5]);
  ASSERT(0 == a.data[6]);
  ASSERT(0 == a.data[7]);

  UInt256 fullValAlpha = uint256_create_from_hex("aaaaaaaa");
  ASSERT(2863311530 == fullValAlpha.data[0]);
  ASSERT(0 == fullValAlpha.data[1]);
  ASSERT(0 == fullValAlpha.data[2]);
  ASSERT(0 == fullValAlpha.data[3]);
  ASSERT(0 == fullValAlpha.data[4]);
  ASSERT(0 == fullValAlpha.data[5]);
  ASSERT(0 == fullValAlpha.data[6]);
  ASSERT(0 == fullValAlpha.data[7]);

  UInt256 fullValNum = uint256_create_from_hex("22222222");
  ASSERT(572662306 == fullValNum.data[0]);
  ASSERT(0 == fullValNum.data[1]);
  ASSERT(0 == fullValNum.data[2]);
  ASSERT(0 == fullValNum.data[3]);
  ASSERT(0 == fullValNum.data[4]);
  ASSERT(0 == fullValNum.data[5]);
  ASSERT(0 == fullValNum.data[6]);
  ASSERT(0 == fullValNum.data[7]);

  UInt256 alphaNum = uint256_create_from_hex("abcd1234");
  ASSERT(2882343476 == alphaNum.data[0]);
  ASSERT(0 == alphaNum.data[1]);
  ASSERT(0 == alphaNum.data[2]);
  ASSERT(0 == alphaNum.data[3]);
  ASSERT(0 == alphaNum.data[4]);
  ASSERT(0 == alphaNum.data[5]);
  ASSERT(0 == alphaNum.data[6]);
  ASSERT(0 == alphaNum.data[7]);

  UInt256 overAlphaNum = uint256_create_from_hex("11abcd1234");
  ASSERT(2882343476 == overAlphaNum.data[0]);
  ASSERT(17 == overAlphaNum.data[1]);
  ASSERT(0 == overAlphaNum.data[2]);
  ASSERT(0 == overAlphaNum.data[3]);
  ASSERT(0 == overAlphaNum.data[4]);
  ASSERT(0 == overAlphaNum.data[5]);
  ASSERT(0 == overAlphaNum.data[6]);
  ASSERT(0 == overAlphaNum.data[7]);

  UInt256 overSameNum = uint256_create_from_hex("2222222222");
  ASSERT(572662306 == overSameNum.data[0]);
  ASSERT(34 == overSameNum.data[1]);
  ASSERT(0 == overSameNum.data[2]);
  ASSERT(0 == overSameNum.data[3]);
  ASSERT(0 == overSameNum.data[4]);
  ASSERT(0 == overSameNum.data[5]);
  ASSERT(0 == overSameNum.data[6]);
  ASSERT(0 == overSameNum.data[7]);

  UInt256 overDiffNum = uint256_create_from_hex("1043658701");
  ASSERT(1130727169 == overDiffNum.data[0]);
  ASSERT(16 == overDiffNum.data[1]);
  ASSERT(0 == overDiffNum.data[2]);
  ASSERT(0 == overDiffNum.data[3]);
  ASSERT(0 == overDiffNum.data[4]);
  ASSERT(0 == overDiffNum.data[5]);
  ASSERT(0 == overDiffNum.data[6]);
  ASSERT(0 == overDiffNum.data[7]);

  UInt256 endZero = uint256_create_from_hex("a5500");
  ASSERT(677120 == endZero.data[0]);
  ASSERT(0 == endZero.data[1]);
  ASSERT(0 == endZero.data[2]);
  ASSERT(0 == endZero.data[3]);
  ASSERT(0 == endZero.data[4]);
  ASSERT(0 == endZero.data[5]);
  ASSERT(0 == endZero.data[6]);
  ASSERT(0 == endZero.data[7]);

  UInt256 startZero = uint256_create_from_hex("00a55");
  ASSERT(2645 == startZero.data[0]);
  ASSERT(0 == startZero.data[1]);
  ASSERT(0 == startZero.data[2]);
  ASSERT(0 == startZero.data[3]);
  ASSERT(0 == startZero.data[4]);
  ASSERT(0 == startZero.data[5]);
  ASSERT(0 == startZero.data[6]);
  ASSERT(0 == startZero.data[7]);

  UInt256 overSameAlpha = uint256_create_from_hex("bbbbbbbbbb");
  ASSERT(3149642683 == overSameAlpha.data[0]);
  ASSERT(187 == overSameAlpha.data[1]);
  ASSERT(0 == overSameAlpha.data[2]);
  ASSERT(0 == overSameAlpha.data[3]);
  ASSERT(0 == overSameAlpha.data[4]);
  ASSERT(0 == overSameAlpha.data[5]);
  ASSERT(0 == overSameAlpha.data[6]);
  ASSERT(0 == overSameAlpha.data[7]);

  UInt256 overDiffAlpha = uint256_create_from_hex("abcdefabcd");
  ASSERT(3455036365 == overDiffAlpha.data[0]);
  ASSERT(171 == overDiffAlpha.data[1]);
  ASSERT(0 == overDiffAlpha.data[2]);
  ASSERT(0 == overDiffAlpha.data[3]);
  ASSERT(0 == overDiffAlpha.data[4]);
  ASSERT(0 == overDiffAlpha.data[5]);
  ASSERT(0 == overDiffAlpha.data[6]);
  ASSERT(0 == overDiffAlpha.data[7]);

  UInt256 midZero = uint256_create_from_hex("abc00123");
  ASSERT(2881487139 == midZero.data[0]);
  ASSERT(0 == midZero.data[1]);
  ASSERT(0 == midZero.data[2]);
  ASSERT(0 == midZero.data[3]);
  ASSERT(0 == midZero.data[4]);
  ASSERT(0 == midZero.data[5]);
  ASSERT(0 == midZero.data[6]);
  ASSERT(0 == midZero.data[7]);

  UInt256 midZeroAcross = uint256_create_from_hex("abcdef0000123456");
  ASSERT(1193046 == midZeroAcross.data[0]);
  ASSERT(2882400000 == midZeroAcross.data[1]);
  ASSERT(0 == midZeroAcross.data[2]);
  ASSERT(0 == midZeroAcross.data[3]);
  ASSERT(0 == midZeroAcross.data[4]);
  ASSERT(0 == midZeroAcross.data[5]);
  ASSERT(0 == midZeroAcross.data[6]);
  ASSERT(0 == midZeroAcross.data[7]);

  UInt256 fullMidZeroIndex = uint256_create_from_hex("aaaaaaaa00000000bbbbbbbb");
  ASSERT(3149642683 == fullMidZeroIndex.data[0]);
  ASSERT(0 == fullMidZeroIndex.data[1]);
  ASSERT(2863311530 == fullMidZeroIndex.data[2]);
  ASSERT(0 == fullMidZeroIndex.data[3]);
  ASSERT(0 == fullMidZeroIndex.data[4]);
  ASSERT(0 == fullMidZeroIndex.data[5]);
  ASSERT(0 == fullMidZeroIndex.data[6]);
  ASSERT(0 == fullMidZeroIndex.data[7]);

  
  UInt256 overMax = uint256_create_from_hex("fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, overMax);
  
  UInt256 max = uint256_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, max);

}

void test_format_as_hex(TestObjs *objs) {
  char *s;
    
  s = uint256_format_as_hex(objs->zero);
  ASSERT(0 == strcmp("0", s));
  free(s);

  s = uint256_format_as_hex(objs->one);
  ASSERT(0 == strcmp("1", s));
  free(s);

  UInt256 alphaNum;
  uint32_t alphaNum_data[8] = { 0xABCD1234U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(alphaNum, alphaNum_data);
  s = uint256_format_as_hex(alphaNum);
  ASSERT(0 == strcmp("abcd1234", s));
  free(s);

  UInt256 overAlphaNum;
  uint32_t overAlphaNum_data[8] = { 0xABCD1234U, 0x11U, 0U, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(overAlphaNum, overAlphaNum_data);
  s = uint256_format_as_hex(overAlphaNum);
  ASSERT(0 == strcmp("11abcd1234", s));
  free(s);
    
  UInt256 endZero;
  uint32_t endZero_data[8] = { 0xA5500U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(endZero, endZero_data);
  s = uint256_format_as_hex(endZero);
  ASSERT(0 == strcmp("a5500", s));
  free(s);
    
  UInt256 startZero;
  uint32_t startZero_data[8] = {0x00A55U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(startZero, startZero_data);
  s = uint256_format_as_hex(startZero);
  ASSERT(0 == strcmp("a55", s));
  free(s);

  UInt256 midZero;
  uint32_t midZero_data[8] = { 0xABC00123U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(midZero, midZero_data);
  s = uint256_format_as_hex(midZero);
  ASSERT(0 == strcmp("abc00123", s));
  free(s);

  UInt256 midZeroAcross;
  uint32_t midZeroAcross_data[8] = { 0x00123456U, 0xABCDEF00U, 0U, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(midZeroAcross, midZeroAcross_data);
  s = uint256_format_as_hex(midZeroAcross);
  ASSERT(0 == strcmp("abcdef0000123456", s));
  free(s);

  UInt256 fullMidZeroIndex;
  uint32_t fullMidZeroIndex_data[8] = { 0xBBBBBBBBU, 0U, 0xAAAAAAAAU, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(fullMidZeroIndex, fullMidZeroIndex_data);
  s = uint256_format_as_hex(fullMidZeroIndex);
  ASSERT(0 == strcmp("aaaaaaaa00000000bbbbbbbb", s));
  free(s);

  UInt256 valsInMid;
  uint32_t valsInMid_data[8] = { 0U, 0x12AB34CDU, 0U, 0U, 0x12AB34CDU, 0U, 0U, 0U };
  INIT_FROM_ARR(valsInMid, valsInMid_data);
  s = uint256_format_as_hex(valsInMid);
  ASSERT(0 == strcmp("12ab34cd000000000000000012ab34cd00000000",s ));
  free(s);
  
  UInt256 zeroEndHex;
  uint32_t zeroEndHex_data[8] = {0x10U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(zeroEndHex, zeroEndHex_data);
  s = uint256_format_as_hex(zeroEndHex);
  ASSERT(0 == strcmp("10", s));
  free(s);
  
  UInt256 rot;
  uint32_t rot_data[8] = { 0x000000ABU, 0U, 0U, 0U, 0U, 0U, 0U, 0xCD000000U };
  INIT_FROM_ARR(rot, rot_data);
  s = uint256_format_as_hex(rot);
  ASSERT(0 == strcmp("cd000000000000000000000000000000000000000000000000000000000000ab", s));
  free(s);
    
  s = uint256_format_as_hex(objs->max);
  ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s));
  free(s);
}

void test_add(TestObjs *objs) {
  UInt256 result;

  result = uint256_add(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_add(objs->zero, objs->one);
  ASSERT_SAME(objs->one, result);

  result = uint256_add(objs->one, objs->zero);
  ASSERT_SAME(objs->one, result);

  UInt256 max;
  for (int i = 0; i < 8; ++i) {
    max.data[i] = ~(0U);
  }
  UInt256 one = uint256_create_from_u32(1UL);
  UInt256 sum = uint256_add(max, one);
  // these assertions should succeed
  ASSERT(sum.data[7] == 0U);
  ASSERT(sum.data[6] == 0U);
  ASSERT(sum.data[5] == 0U);
  ASSERT(sum.data[4] == 0U);
  ASSERT(sum.data[3] == 0U);
  ASSERT(sum.data[2] == 0U);
  ASSERT(sum.data[1] == 0U);
  ASSERT(sum.data[0] == 0U);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_add(objs->one, objs->one);
  ASSERT_SAME(two, result);

  result = uint256_add(objs->max, objs->one);
  ASSERT_SAME(objs->zero, result);
}

void test_sub(TestObjs *objs) {
  UInt256 result;

  result = uint256_sub(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->one, objs->one);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->zero, objs->one);
  ASSERT_SAME(objs->max, result);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  uint32_t three_data[8] = { 3U };
  UInt256 three;
  INIT_FROM_ARR(three, three_data);
  result = uint256_sub(three, two);
  ASSERT_SAME(objs->one, result);
}

void test_negate(TestObjs *objs) {
  UInt256 result;

  result = uint256_negate(objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_negate(objs->one);
  ASSERT_SAME(objs->max, result);

  result = uint256_negate(objs->max);
  ASSERT_SAME(objs->one, result);

  //uint32_t two_data[8] = { 2U };
  //UInt256 two;
  //INIT_FROM_ARR(two, two_data);
  //uint32_t ans_data[8] = { 254U };
  //UInt256 ans;
  //INIT_FROM_ARR(ans, ans_data);
  //result = uint256_negate(two);
  //ASSERT_SAME(ans, result);

}

void test_rotate_left(TestObjs *objs) {
  UInt256 result;

  // rotating the value with just the most significant bit set
  // one position to the left should result in the value equal to 1
  // (i.e., the value with only the least significant bit set)
  result = uint256_rotate_left(objs->msb_set, 1);
  ASSERT_SAME(objs->one, result);

  result = uint256_rotate_left(objs->msb_set, 257);
  ASSERT_SAME(objs->one, result);
  
  result = uint256_rotate_left(objs->max, 2);
  ASSERT_SAME(objs->max, result);

  // after rotating the "rot" value left by 4 bits, the resulting value should be
  //   D0000000 00000000 00000000 00000000 00000000 00000000 00000000 00000ABC
  result = uint256_rotate_left(objs->rot, 4);
  ASSERT(0x00000ABCU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xD0000000U == result.data[7]);

  result = uint256_rotate_left(objs->rot, 32);
  ASSERT(0U == result.data[7]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[2]);
  ASSERT(0x000000ABU == result.data[1]);
  ASSERT(0xCD000000U == result.data[0]);

  result = uint256_rotate_left(objs->rot, 256);
  ASSERT(0xCD000000U == result.data[7]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[1]);
  ASSERT(0x000000ABU == result.data[0]);
  
  result = uint256_rotate_left(objs->zero, 4);
  ASSERT(0U == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);

  UInt256 alphaNum;
  uint32_t alphaNum_data[8] = { 0xABCD1234U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(alphaNum, alphaNum_data);
  result = uint256_rotate_left(alphaNum, 4);
  ASSERT(0xBCD12340U == result.data[0]);
  ASSERT(0x0000000AU == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);

  result = uint256_rotate_left(alphaNum, 3);
  ASSERT(0x5E6891A0U == result.data[0]);
  ASSERT(0x00000005U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);

  result = uint256_rotate_left(alphaNum, 64);
  ASSERT(0U == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0xABCD1234U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);
}

void test_rotate_right(TestObjs *objs) {
  UInt256 result;

  // rotating 1 right by 1 position should result in a value with just
  // the most-significant bit set
  result = uint256_rotate_right(objs->one, 1);
  ASSERT_SAME(objs->msb_set, result);

  result = uint256_rotate_right(objs->one, 257);
  ASSERT_SAME(objs->msb_set, result);

  // after rotating the "rot" value right by 4 bits, the resulting value should be
  //   BCD00000 00000000 00000000 00000000 00000000 00000000 00000000 0000000A
  result = uint256_rotate_right(objs->rot, 4);
  ASSERT(0x0000000AU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xBCD00000U == result.data[7]);

  result = uint256_rotate_right(objs->rot, 256);
  ASSERT_SAME(objs->rot, result);

  result = uint256_rotate_right(objs->rot, 3);
  ASSERT(0x79A00000 == result.data[7]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[1]);
  ASSERT(0x15U == result.data[0]);

  UInt256 alphaNum;
  uint32_t alphaNum_data[8] = { 0xABCD1234U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  INIT_FROM_ARR(alphaNum, alphaNum_data);
  result = uint256_rotate_right(alphaNum, 4);
  ASSERT(0xABCD123U == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0x40000000U == result.data[7]);

  result = uint256_rotate_right(alphaNum, 64);
  ASSERT(0U == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0xABCD1234U == result.data[6]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[7]);
}
