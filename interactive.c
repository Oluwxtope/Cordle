// This program allows you to play the Wordle game interactively in the console
// Note: you will first need to implement the wordle.c module :-) 

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordle.h"
#include "wordlist.h"
#include <curl/curl.h>

struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

int main(void) {
  char guess1[100] = "";
  char guess2[100] = "";
  char guess3[100] = "";
  char guess4[100] = "";
  char guess5[100] = "";
  char guess6[100] = "";
  char result1[100] = "";
  char result2[100] = "";
  char result3[100] = "";
  char result4[100] = "";
  char result5[100] = "";
  char result6[100] = "";
  char *guesses[6] = {guess1, guess2, guess3, guess4, guess5, guess6};
  char *results[6] = {result1, result2, result3, result4, result5, result6};
  char alphabet[27] = "";
  
  char mode = '\0';
  char secret_word[6] = {0};
  int difficulty = 0;

  printf("Please choose your mode: (w)ordlist or (a)pi? ");
  int result = scanf("%c", &mode);
  if (result != 1 || !(mode == 'w' || mode == 'a')) {
    printf("INVALID MODE\n");
    exit(1);
  } else if (mode == 'w') {
    int word_idx = -1;
    printf("Please enter a valid word number between 0 and %d\n",
        wordle_word_list_len - 1);
    result = scanf("%d", &word_idx);
    if (result != 1 || word_idx < 0 || word_idx >= wordle_word_list_len) {
      printf("INVALID WORD NUMBER\n");
      exit(1);
    }
    strcpy(secret_word, wordle_word_list[word_idx]);
    printf("Please enter a difficulty level (0 = easy, 1 = normal, 2 = hard)\n");
    result = scanf("%d", &difficulty);
    if (result != 1 || difficulty < 0 || difficulty > 2) {
      printf("INVALID DIFFICULTY\n");
    exit(1);
    }
  } else if (mode == 'a') {
    CURL *hnd = curl_easy_init();
    if (!hnd) {
        fprintf(stderr, "init failed");
        return EXIT_FAILURE;
    }
    struct string s;
    init_string(&s);

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_URL, "https://random-words5.p.rapidapi.com/getRandom?wordLength=5");
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &s);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "X-RapidAPI-Host: random-words5.p.rapidapi.com");
    headers = curl_slist_append(headers, "X-RapidAPI-Key: <INSERT KEY FROM API.txt>");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    CURLcode ret = curl_easy_perform(hnd);
    if (ret != CURLE_OK) {
        fprintf(stderr, "download problem: %s\n", curl_easy_strerror(ret));
    }
    
    strcpy(secret_word, s.ptr);
    assert(strlen(secret_word) == 5);
    curl_easy_cleanup(hnd);
    free(s.ptr);

    printf("Please enter a difficulty level (0 = normal, 1 = hard)\n");
    result = scanf("%d", &difficulty);
    if (result != 1 || difficulty < 0 || difficulty > 1) {
      printf("INVALID DIFFICULTY\n");
    exit(1);
    }
  }
  
  
  
  int num_guesses = 0;
  while (1) {
    char *cur_word = guesses[num_guesses];
    printf("----------------------------------------------------\n");
    for (int i = 0; i < num_guesses; ++i) {
      printf("  [%d] %s => %s\n", i + 1, guesses[i], results[i]);
    }
    available_letters(guesses, results, num_guesses, alphabet);
    printf("  %s\n", alphabet);
    printf("Enter your guess #%d:\n", num_guesses + 1);
    
    result = scanf("%s", cur_word);
    if (result == EOF) {
      printf("Exiting (EOF detected)\n");
      exit(1);
    }
    if (strlen(cur_word) != 5) {
      printf("WRONG NUMBER OF LETTERS\n");
      continue;
    }
    if (difficulty > 0 && mode == 'w') {
      if (find_in_list(cur_word, wordle_word_list, wordle_word_list_len) == -1) {
        printf("WORD NOT IN DICTIONARY\n");
        continue;
      }
    }
    if (difficulty == 2 || (difficulty == 1 & mode == 'a')) {
      if (!valid_hard_guess(guesses, results, num_guesses, cur_word)) {
        printf("INVALID GUESS (HARD MODE)\n");
        continue;
      }
    }
    if (evaluate_guess(secret_word, cur_word, results[num_guesses])) {
      printf("YOU WIN! The word was: %s\n", secret_word);
      exit(0);
    }
    ++num_guesses;
    if (num_guesses == 6) {
      printf("YOU LOSE! The word was: %s\n", secret_word);
      exit(0);
    }
  }
}
