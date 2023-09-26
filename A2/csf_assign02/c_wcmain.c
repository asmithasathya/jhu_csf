#include <stdio.h>
#include <stdint.h>
#include "wcfuncs.h"
#include <stdlib.h>

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249


int main(int argc, char **argv) {
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  const unsigned char *best_word = (const unsigned char *) "";
  uint32_t best_word_count = 0;

  // read input file or retrieve input from standard input
  FILE *in_file;
  if (argc >= 2) {
    in_file = fopen(argv[1], "r");
    if (!in_file) {
      fprintf(stderr, "Error: Cannot open file\n");
      return 1;
    }
  }
  else {
    in_file = stdin;
  }

  // create hashtable
  struct WordEntry *words[HASHTABLE_SIZE] = { NULL };

  unsigned char curr_word[MAX_WORDLEN + 1] = "";
  unsigned char best_word_temp[MAX_WORDLEN + 1] = "";

  // read through all the words in the input and save the best word with the highest count
  while (wc_readnext(in_file, curr_word)) {
    total_words++;
    wc_tolower(curr_word);
    wc_trim_non_alpha(curr_word);
    struct WordEntry *current = wc_dict_find_or_insert(words, HASHTABLE_SIZE, curr_word);
    current->count++;
    if (current->count == 1) {
      unique_words++;
    }
    // when the current word has the highest count
    if (current->count > best_word_count) {
      best_word_count = current->count;
      wc_str_copy(best_word_temp, current->word);
    }
    // when there are multiple words with the same highest count
    else if (current->count == best_word_count) {
      if (wc_str_compare(current->word, best_word_temp) < 0) {
	wc_str_copy(best_word_temp, current->word);
      }
    }
 }

  best_word = best_word_temp;

  for (unsigned i = 0; i < HASHTABLE_SIZE; i++) {
    wc_free_chain(words[i]);
  }
  
  printf("Total words read: %u\n", (unsigned int) total_words);
  printf("Unique words read: %u\n", (unsigned int) unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *) best_word, best_word_count);

  if (in_file != stdin) {
    fclose(in_file);
  }

  return 0;
}
