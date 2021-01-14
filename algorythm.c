/*
 *
 *
 */

#include "ch.h"
#include "hal.h"

#include "shell.h"
#include "chprintf.h"
#include "ssd1306.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

static char* morse[] = {"-----",".----","..---","...--","....-", ".....", "-....","--...","---..","----.",
                      ".-","-...","-.-.","-..",".","..-.","--.","....","..",".---", "-.-",".-..","--","-.","---",".--.","--.-",".-.","...","-","..-", "...-",".--","-..-","-.--","--.."};char * tx_buffer =
                          ".-.-...--....         \0";


char* charToMorse(char c){

  if (c > 47 && c < 58)
    return morse[c - '0'];
  else if(c > 64 && c < 91)
    return morse[c - 'A' + 10];
  else if(c > 96 && c < 123)
      return morse[c - 'a' + 10];
  else
    return "?";
}

typedef struct {
  char alfa;
  char morse[5];
}convert;

convert conv[26] = {
                      {'a',{'.','-','\0'}},
                      {'b',{'-','.','.','.','\0'}},
                      {'c',{'-','.','-','.','\0'}},
                      {'d',{'-','.','.','\0'}},
                      {'e',{'.','\0'}},
                      {'f',{'.','.','-','.','\0'}},
                      {'g',{'-','-','.','\0'}},
                      {'h',{'.','.','.','.','\0'}},
                      {'i',{'.','.','\0'}},
                      {'j',{'.','-','-','-','\0'}},
                      {'k',{'-','.','-','\0'}},
                      {'l',{'.','-','.','.','\0'}},
                      {'m',{'-','-','\0'}},
                      {'n',{'-','.','\0'}},
                      {'o',{'-','-','-','\0'}},
                      {'p',{'.','-','-','.','\0'}},
                      {'q',{'-','-','.','-','\0'}},
                      {'r',{'.','-','.','\0'}},
                      {'s',{'.','.','.','\0'}},
                      {'t',{'-','\0'}},
                      {'u',{'.','.','-','\0'}},
                      {'v',{'.','.','.','-','\0'}},
                      {'w',{'.','-','-','\0'}},
                      {'x',{'-','.','.','-','\0'}},
                      {'y',{'-','.','-','-','\0'}},
                      {'z',{'-','-','.','.','\0'}}
  };


char morseToChar(char buffer[], convert conv[], BaseSequentialStream *chp){
  int notFound = 0;
  for(int n =0;n<26;++n){
    if (strcmp(buffer,conv[n].morse)==0){
      //chprintf(chp, "%c",conv[i].alfa);
      notFound=1;
      return conv[n].alfa;
    }
  }

  if(notFound ==0){
    chprintf(chp,"%s","Questa stringa non codifica nessuna parola in morse \n");
    return '\0';

  }

}



