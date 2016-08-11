/*
* Copyright 2016 Erint Labs OÜ
*
* Licensed under the EUPL, Version 1.1 or – as soon they
* will be approved by the European Commission - subsequent
* versions of the EUPL (the "Licence");
* You may not use this work except in compliance with the
* Licence.
* You may obtain a copy of the Licence at:
*
* https://joinup.ec.europa.eu/software/page/eupl
*
* Unless required by applicable law or agreed to in
* writing, software distributed under the Licence is
* distributed on an "AS IS" basis,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied.
* See the Licence for the specific language governing
* permissions and limitations under the Licence.
*/
#include <check.h>
#include <stdlib.h>
#include "kalyna.h"

/* TODO: check if endinanness might be an issue */
/* TODO: some verbosity on why exactly have the test failed */
/* TODO: reorganise nomenclature perhaps */

/* Base function for testing hash functions, currently only ГОСТ 34.311 is supported */
const char* hash_template(const uint8_t *T, size_t LT, const uint8_t *ref_HT, size_t ref_LH)
{
  /* Check if we compare to the hash of correct length */
  size_t LH = dstu4145_hashlen(dstu4145_defaultiH, dstu4145_defaultLiH);
  if (ref_LH != LH)
    return "Reported hash length does not match reference";

  /* Compute the hash */
  uint8_t *HT = malloc(LH);
  const char *reason = NULL;
  if (!dstu4145_hashmessage(T, LT, dstu4145_defaultiH, dstu4145_defaultLiH, HT, LH))
    reason = "Hash function failed";

  /* If computation was successful, compare it to reference */
  if (!reason)
    if (memcmp(HT, ref_HT, LH))
      reason = "Computed hash does not match the reference";

  /* Clean up and return */
  free(HT);
  return reason;
}

/* Test vectors taken from Kalyna documentation, annex B.2 */
START_TEST(kalyna_test_b_2_2_1)
{
  /* 128-bit key expansion for 128-bit block encryption/decryption */
  const uint8_t key[] = {   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
                            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, };
                            
  /* Reference round keys */
  const uint8_t rkeys[] = {
      /* Round 0 */
      0x16, 0x50, 0x5E, 0x6B, 0x9B, 0x3A, 0xB1, 0xE6, 0x86, 0x5B, 0x77, 0xDC, 0xE0, 0x82, 0xA0, 0xF4, 
      /* Round 1 */
      0xE6, 0x86, 0x5B, 0x77, 0xDC, 0xE0, 0x82, 0xA0, 0xF4, 0x16, 0x50, 0x5E, 0x6B, 0x9B, 0x3A, 0xB1, 
      /* Round 2 */
      0x7E, 0x70, 0x87, 0x6E, 0xAE, 0x49, 0x84, 0x76, 0x8A, 0xAA, 0xA0, 0x0A, 0x7C, 0x93, 0xEC, 0x42, 
      /* Round 3 */
      0x76, 0x8A, 0xAA, 0xA0, 0x0A, 0x7C, 0x93, 0xEC, 0x42, 0x7E, 0x70, 0x87, 0x6E, 0xAE, 0x49, 0x84, 
      /* Round 4 */
      0x45, 0xCE, 0xD4, 0xC5, 0x1E, 0x91, 0x40, 0xF5, 0x3E, 0x72, 0x76, 0x82, 0x0F, 0x0B, 0xD9, 0xFE, 
      /* Round 5 */
      0xF5, 0x3E, 0x72, 0x76, 0x82, 0x0F, 0x0B, 0xD9, 0xFE, 0x45, 0xCE, 0xD4, 0xC5, 0x1E, 0x91, 0x40, 
      /* Round 6 */
      0x8C, 0x77, 0xEE, 0x22, 0x79, 0x00, 0xC4, 0x62, 0x51, 0x5F, 0x66, 0x32, 0x05, 0x60, 0xC4, 0xB1, 
      /* Round 7 */
      0x62, 0x51, 0x5F, 0x66, 0x32, 0x05, 0x60, 0xC4, 0xB1, 0x8C, 0x77, 0xEE, 0x22, 0x79, 0x00, 0xC4, 
      /* Round 8 */
      0x0A, 0x98, 0x72, 0xE2, 0x5C, 0xD2, 0xB0, 0xB8, 0xAA, 0x87, 0x9A, 0x20, 0x86, 0xA6, 0x6D, 0xD8, 
      /* Round 9 */
      0xB8, 0xAA, 0x87, 0x9A, 0x20, 0x86, 0xA6, 0x6D, 0xD8, 0x0A, 0x98, 0x72, 0xE2, 0x5C, 0xD2, 0xB0, 
      /* Round 10 */
      0x57, 0x26, 0xB1, 0xA8, 0x94, 0xDB, 0xC4, 0x18, 0xF6, 0x0B, 0xF3, 0xD5, 0xE8, 0xD7, 0x48, 0x61, 
  };

  const char * reason = kalyna_keyexp_template(key, 128, rkeys, 11);
  fail_if(reason != NULL, reason);
}
END_TEST

START_TEST(gost_34_311_hash_test_a_3_2)
{
  /* "Suppose the original message has length = 50 bytes" */
  const uint8_t T[] =   { 0x73, 0x65,   0x74, 0x79, 0x62, 0x20,   0x30, 0x35, 0x20, 0x3D,
                                        0x20, 0x68, 0x74, 0x67,   0x6E, 0x65, 0x6C, 0x20,
                                        0x73, 0x61, 0x68, 0x20,   0x65, 0x67, 0x61, 0x73,
                                        0x73, 0x65, 0x6D, 0x20,   0x6C, 0x61, 0x6E, 0x69,
                                        0x67, 0x69, 0x72, 0x6F,   0x20, 0x65, 0x68, 0x74,
                                        0x20, 0x65, 0x73, 0x6F,   0x70, 0x70, 0x75, 0x53, };

  const uint8_t HT[] =  { 0x08, 0x52, 0xF5, 0x62,   0x3B, 0x89, 0xDD, 0x57,
                          0xAE, 0xB4, 0x78, 0x1F,   0xE5, 0x4D, 0xF1, 0x4E,
                          0xEA, 0xFB, 0xC1, 0x35,   0x06, 0x13, 0x76, 0x3A,
                          0x0D, 0x77, 0x0A, 0xA6,   0x57, 0xBA, 0x1A, 0x47, };

  const char * reason = hash_template(T, sizeof(T), HT, sizeof(HT));
  fail_if(reason != NULL, reason);
}
END_TEST

Suite *dstu4145_suite() {
  Suite *suite = suite_create("Ciphers");
  TCase *tcase_kalyna = tcase_create("Kalyna");
  tcase_add_test(tcase_gost, gost_34_311_hash_test_a_3_1);
  tcase_add_test(tcase_gost, gost_34_311_hash_test_a_3_2);
  suite_add_tcase(suite, tcase_kalyna);
  
//   TCase *tcase_gost = tcase_create("GOST 34.311");
//   tcase_add_test(tcase_gost, gost_34_311_hash_test_a_3_1);
//   tcase_add_test(tcase_gost, gost_34_311_hash_test_a_3_2);
//   suite_add_tcase(suite, tcase_gost);

  return suite;
}

int main(int argc, char** argv)
{
  int failed;
  Suite *s = dstu4145_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return failed;
}
