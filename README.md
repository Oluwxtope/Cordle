# Cordle
<i> Wordle, but with C! </i>

## Version
1.0
1.1: Added internet mode where you generate word by making API call (needs internet connection)

## Setup
1. Navigate to the directory with Cordle source code files in terminal
2. To compile source code files, run `gcc wordlist.c wordle.c interactive.c -lcurl`
3. Enter `./a.out` in terminal to play Cordle
4. P.S. Program doesn't run as is because of missing API key which I have hidden! To run, subscribe to Random Words API on rapidapi.com and insert your API key in line 100 of `interactive.c`. Or contact me :)

## Instructions
1. Set up your game by picking your mode and difficulty level
2. Type in your 5-letter word guess
3. If your guess is correct, YOU WIN!
4. If your guess is incorrect, Cordle shows you the result of your guess:

    > [1] great => ..eA.  
    > AbcdEf.hijklmnopq.s.uvwxyz  

    This means the word doesn't contain the letters 'g', 'r', or 't'. It contains the letters 'e' and 'a' but 'e' is in an incorrect spot hence the lowercase while 'a' is in the correct spots which is why it's capitalized in the result of your guess. The alphabet string below shows which letters are for sure in the word by capitalizing them. The lowercase letters may be in the word, while the dots represent letters not present in the word at all 
5. In hard mode, you cannot use letters not in the word in subsequent guesses. You also can't use a letter in the wrong spot in subsequent guesses. 

## Future Updates
1. On hard using (a)pi mode, add verification of guesses by making spellcheck api call