
/*
 *
 * CS-252 Fall 2017
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.hh"
#include <unistd.h>
#include <fcntl.h>

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



source.* {
  char *src = strdup(yytext);
  src += 7;
  char line[256];
  char *buffer = line;
  char ch = 0;
  int inFile = open(src, O_RDONLY);
  
  while (read(inFile, &ch, 1)) *buffer++ = ch;
  while (buffer >= line) unput(*buffer--);
}

[`][^\n\`]*[`] {
  yytext[strlen(yytext+1)] ='\0';
  char *command = strdup(yytext+1);
  
  int tmpin = dup(0);
  int tmpout = dup(1);
  int fdpipein[2];
  int fdpipeout[2];
  
  pipe(fdpipein);
  pipe(fdpipeout);

  write(fdpipein[1], command, strlen(command));
  write(fdpipein[1], "\n", 1);
  write(fdpipein[1], "exit", 4);
  write(fdpipein[1], "\n", 1);

  close(fdpipein[1]);

  dup2(fdpipein[0], 0);
  close(fdpipein[0]);
  dup2(fdpipeout[1], 1);
  close(fdpipeout[1]);

  int ret = fork();
  if (ret == 0) {
    execvp("/proc/self/exe", NULL);
    _exit(1);
  } else if (ret < 0) {
    perror("fork");
    exit(1);
  }
  
  dup2(tmpin, 0);
  dup2(tmpout, 1);
  close(tmpin);
  close(tmpout);

  char ch;
  char *buffer = (char *) malloc(100);
  int i = 0;

  while (read(fdpipeout[0], &ch, 1)) {
    if (ch == '\n') buffer[i++] = ' ';
    else buffer[i++] = ch;
  }
  buffer[i] = '\0';
  for (i = strlen(buffer); i >= 0; i--)
    unput(buffer[i]);
  //Command::clear();
  //Command::prompt();
}

["][^\n\"]*["] {
  yytext[strlen(yytext +1)] = '\0';
  yylval.string_val = strdup(yytext+1);
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
