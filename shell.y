
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%code requires 
{
#include <string>

#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <string_val> WORD
%token NOTOKEN GREAT NEWLINE GREATGREATAMPERSAND GREATGREAT GREATAMPERSAND AMPERSAND PIPE LESS

%{
//#define yylex yylex
#include <cstdio>
#include "command.hh"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <signal.h>

void yyerror(const char * s);
int yylex();

%}

%%

goal:
  commands
  ;
  
commands:
  command
  | commands command
  ;

command: simple_command
       ;

simple_command:	
  pipe_list iomodifier_list background_optional NEWLINE {
    printf("   Yacc: Execute command\n");
    Command::_currentCommand.execute();
  }
  | NEWLINE {
  }
  | error NEWLINE { yyerrok; }
  ;

command_and_args:
  command_word argument_list {
    Command::_currentCommand.
      insertSimpleCommand( Command::_currentSimpleCommand );
  }
  ;

argument_list:
  argument_list argument
  | /* can be empty */
  ;

argument:
  WORD {
    printf("   Yacc: insert argument \"%s\"\n", $1);
    //eWCIN($1)
    Command::_currentSimpleCommand->insertArgument( $1 );\
  }
  ;

command_word:
  WORD {
    printf("   Yacc: insert command \"%s\"\n", $1);
    Command::_currentSimpleCommand = new SimpleCommand();
    Command::_currentSimpleCommand->insertArgument( $1 );
  }
  ;

pipe_list:
  pipe_list PIPE command_and_args
  | command_and_args
  ;

iomodifier_opt:
  GREAT WORD {
    printf("   Great Word Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._outFile = strdup($2);
    Command::_currentCommand._outCounter++;
  }
  | GREATGREAT WORD {
    printf("   Great Great Word Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._outFile = strdup($2);
    Command::_currentCommand._append = 1;
    Command::_currentCommand._outCounter++;
  }
  | GREATAMPERSAND WORD {
    printf("   Great Ampersand Word Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._outfile = strdup($2);
    Command::_currentCommand._errFile = strdup($2);
    Command::_currentCommand._outCounter++;
  }
  | GREATGREATAMPERSAND WORD {
    printf("   Great Great Ampersand Word Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._outFile = strdup($2);
    Command::_currentCommand._errFile = strdup($2);
    Command::_currentCommand._outCounter++;
    Command::_currentCommand._append = 1;
  }
  | LESS WORD {
    printf("   Less Word Yacc: insert output \"%s\"\n", $2);
    Command::_currentCommand._inFile = strdup($2);
    Command::_currentCommand._inCounter++;
  }
  ;

iomodifier_list:
  iomodifier_list iomodifier_opt
  | iomodifier_opt
  |
  ;

background_optional:
  AMPERSAND {
  }
  |
  ;

%%

void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}

#if 0
main()
{
  yyparse();
}
#endif
