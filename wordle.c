// This module contains the main functions that power Wordle

#include "wordle.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "cs136-trace.h"

// see wordle.h
bool evaluate_guess(const char *secret, const char *guess, char *result) {
  assert(secret);
  assert(guess);
  assert(result);
  
  int secret_len = strlen(secret);
  int guess_len = strlen(guess);
  assert(secret_len == guess_len);
  
  int ascii_array[128] = {0};
  const char *secret_p = secret;
  while (*secret_p) {
    int secret_ascii = *secret_p;
    ascii_array[secret_ascii] = 1;
    ++secret_p;
  }
  
  secret_p = secret;
  const char *guess_p = guess;
  char *result_p = result;
  bool correct_guess = true;
  while (*secret_p) {
    int guess_ascii = *guess_p;
    if (*guess_p == *secret_p) {
      *result_p = *guess_p - 32;
    } else if (ascii_array[guess_ascii]) {
      *result_p = *guess_p;
      correct_guess = false;
    } else {
      *result_p = '.';
      correct_guess = false;
    }
    ++secret_p;
    ++guess_p;
    ++result_p;
  }
  return correct_guess;
}


// see wordle.h
int find_in_list(const char *guess, char *word_list[], int num_words) {
  assert(guess);
  assert(word_list);
  
  int start = 0;
  int stop = num_words - 1;
  int mid = (stop - start) / 2;
  
  while (start <= stop) {
    if (!strcmp(guess, word_list[mid])) {
      return mid;
    } else if (strcmp(guess, word_list[mid]) > 0) {
      start = mid + 1;
    } else {
      stop = mid - 1;
    }
    mid = (start + stop) / 2;
  }
  return -1;
}


// see wordle.h
void available_letters(char *guesses[], char *results[], int num_guesses,
                       char *alphabet) {
  assert(guesses);
  assert(results);
  assert(num_guesses >= 0);
  assert(alphabet);
  int len = 0;
  if (num_guesses > 0) {
    // assertion loop doesn't violate time complexity
    len = strlen(guesses[0]);
    for (int i = 0; i < num_guesses; ++i) {
      assert(guesses[i]);
      assert(results[i]);
      assert(len == strlen(guesses[i]));
      assert(len == strlen(results[i]));
    }
  }
  
  //test: available 2 funky ..... plane pla.E
  // initialize alphabet string to be filled with lowercase a...z
  // should not change time complexity...
  char *alpha_p = alphabet;
  int alpha_ascii = 97;
  int j = 0;
  while (j < 26) {
    *alpha_p = alpha_ascii;
    ++alpha_ascii;
    ++alpha_p;
    ++j;
  }
  
  for (int i = 0; i < num_guesses; ++i) {
    for (int ltr = 0; ltr < len; ++ltr) {
      if (results[i][ltr] == '.') {
        int char_ascii = guesses[i][ltr];
        int alpha_pos = char_ascii - 97;
        *(alphabet + alpha_pos) = '.';
      } else {
        int char_ascii = guesses[i][ltr];
        int alpha_pos = char_ascii - 97;
        *(alphabet + alpha_pos) = char_ascii - 32;
      }
    }
  } 
}


// see wordle.h
bool valid_hard_guess(char *guesses[], char *results[], int num_guesses,
                      const char *next_guess) {
  assert(guesses);
  assert(results);
  assert(num_guesses >= 0);
  assert(next_guess);
  int len = 0;
  if (num_guesses > 0) {
    // assertion loop doesn't violate time complexity
    len = strlen(guesses[0]);
    for (int i = 0; i < num_guesses; ++i) {
      assert(guesses[i]);
      assert(results[i]);
      assert(len == strlen(guesses[i]));
      assert(len == strlen(results[i]));
    }
  }
  assert(len == strlen(next_guess));
  
  for (int i = 0; i < num_guesses; ++i) {
    for (int ltr = 0; ltr < len; ++ltr) {
      int result_ascii = results[i][ltr];
      if (results[i][ltr] == '.') {
        const char *next_p = next_guess;
        while (*next_p) {
          if (*next_p == guesses[i][ltr]) {
            return false;
          }
          ++next_p;
        }
      } else if (result_ascii >=  97 && result_ascii <=  122) {
        if (*(next_guess + ltr) == guesses[i][ltr]) {
          return false;
        }
        if (1) {
          bool used_in_guess = false;
          const char *next_p = next_guess;
          while (*next_p) {
            if (*next_p == guesses[i][ltr]) {
              used_in_guess = true;
            }
            ++next_p;
          }
          if (!used_in_guess) {
            return false;
          }
        }
      } else {
        if (*(next_guess + ltr) != guesses[i][ltr]) {
          return false;
        }
      }
    }
  }
  return true;
}


// see wordle.h
int find_solution(char *guesses[], char *results[], int num_guesses,
                  char *word_list[], int num_words,
                  char *solutions[], int max_solutions) {
  assert(guesses);
  assert(results);
  assert(num_guesses >= 0);
  assert(word_list);
  assert(max_solutions >= 1);
  int len = 0;
  if (num_guesses > 0) {
    // assertion loop doesn't violate time complexity
    len = strlen(guesses[0]);
    for (int i = 0; i < num_guesses; ++i) {
      assert(guesses[i]);
      assert(results[i]);
      assert(len == strlen(guesses[i]));
      assert(len == strlen(results[i]));
    }
    for (int j = 0; j < num_words; ++j) {
      assert(word_list[j]);
      assert(len == strlen(word_list[j]));
    }
  }
  
  int sol_count = 0;
  for (int i = 0; i < num_words; ++i) {
    char alphabet_seq[27] = "..........................";
    for (int k = 0; k < len; ++k) {
      int char_ascii = word_list[i][k];
      int alpha_pos = char_ascii - 97;
      alphabet_seq[alpha_pos] = char_ascii - 32;
    }
    
    bool valid_guess = true;
    for (int j = 0; j < num_guesses; ++j) {
      for (int k = 0; k < len; ++k) {
        int guess_char_ascii = guesses[j][k];
        int guess_alpha_pos = guess_char_ascii - 97;
        int result_char_ascii = results[j][k];
        //int result_alpha_pos = guess_char_ascii - 97;
        if (result_char_ascii == 46) {
          if (alphabet_seq[guess_alpha_pos] != '.') {
            valid_guess = false;
          }
        } else if (result_char_ascii >= 97 && result_char_ascii <= 122) {
          if (word_list[i][k] == guesses[j][k]) {
            valid_guess = false;
          } else if (alphabet_seq[guess_alpha_pos] == '.') {
            valid_guess = false;
          }
        } else {
          if (word_list[i][k] != guesses[j][k]) {
            valid_guess = false;
          }
        }
      }
    }
    if (valid_guess) {
      solutions[sol_count] = word_list[i];
      ++sol_count;
      if (sol_count == max_solutions) {
        return sol_count;
      }
    }
  }
  return sol_count;
}
