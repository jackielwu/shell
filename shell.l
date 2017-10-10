
/*
 *
 * CS-252 Fall 2017
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <cstring>
#include "y.tab.hh"

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

%}

%%

\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}

">>&" {
  return GREATGREATAMPERSAND;
}

">>" {
  return GREATGREAT;
}

">&" {
  return GREATAMPERSAND;
}

"2>" {
  return TWOGREAT;
}

">" {
  return GREAT;
}

"&" {
  return AMPERSAND;
}

"|" {
  return PIPE;
}

"<" {
  return LESS;
}

["][^\n\"]*["] {
  yytext[strlen(yytext+1)] = '\0';
  yylval.string_val = strdup(yytext +1);
  return WORD;
}

[^ \t\n|><&]*\\[^ \t\n]* {
  int i = 0;
  char *eC = (char *) malloc(200);
  char *temp = yytext;
  while (*temp) {
    if (*temp == '\\') {
      if (*(temp + 1) == '\\') {
        temp += 2;
        eC[i++] = '\\';
      }
      else {
        eC[i++] = *(++temp);
      }
    }
    else {
      eC[i++] = *temp;
    }
    temp++;
  }
  eC[i] = '\0';
  yylval.string_val = strdup(eC);
  return WORD;
}


[^ \t\n|><][^ \t\n|><]*  {
  /* Assume that file names have only alpha chars */
  yylval.string_val = strdup(yytext);
  return WORD;
}
