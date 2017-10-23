#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <stdio.h>

#include "simpleCommand.hh"
#include "command.hh"

SimpleCommand::SimpleCommand() {
	// Create available space for 5 arguments
	_numOfAvailableArguments = 5;
	_numOfArguments = 0;
	_arguments = (char **) malloc( _numOfAvailableArguments * sizeof( char * ) );
}

char *SimpleCommand::envExpansion(char *args) {
  char *str = strdup(args);
  char *dollar = strchr(str, '$');
  char *brace = strchr(str, '{');

  char *replace = (char *) malloc(sizeof(args) +50);
  char *temp = replace;

  if(dollar && brace) {
    while(*str != '$') {
      *temp++ = *str++;
    }
    *temp='\0';
    while(dollar) {
      if(dollar[1] =='{' && dollar[2] != '}') {
        char *temp2 = dollar +2;
        char *env = (char *) malloc(20);
        char *envtemp = env;
        while(*temp2 != '}') {
          *envtemp++ = *temp2++;
        }
        *envtemp = '\0';
        char *get = getenv(env);
        strcat(replace, get);
        while(*(str-1) != '}') str++;
        char *buf = (char *) malloc(20);
        char *tempbuf = buf;
        while(*str != '$' && *str) {
          *tempbuf++ = *str++;
        }
        *tempbuf = '\0';
        strcat(replace, buf);
      }
      dollar++;
      dollar = strchr(dollar, '$');
    }
    args = strdup(replace);
    return args;
  }
  return NULL;
}

char *SimpleCommand::tilde(char *argument) {
  if(argument[0] == '~') {
    _outCounter = 1;
    if(strlen(argument) == 1) {
      argument = strdup(getenv("HOME"));
      return argument;
    }
    else {
      if(argument[1] == '/') {
        char *dir = strdup(getenv("HOME"));
        argument++;
        argument = strcat(dir, argument);
        return argument;
      }
      char *nargs = (char *) malloc(strlen(argument) + 20);
      char *uname = (char *) malloc(50);
      char *user = uname;
      char *temp = argument;

      temp++;
      while(*temp != '/' && *temp) *(uname++) = *(temp++);
      *uname = '\0';
      //printf("%s\n",getpwnam(user)->pw_dir);
      if(*temp) {
        nargs = strcat(getpwnam(user)->pw_dir, temp);
        argument = strdup(nargs);
        return argument;
      }
      else {
        argument = strdup(getpwnam(user)->pw_dir);
        return argument;
      }
    }
  }
  return NULL;
}


void SimpleCommand::insertArgument( char * argument ) {
	if ( _numOfAvailableArguments == _numOfArguments  + 1 ) {
		// Double the available space
		_numOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numOfAvailableArguments * sizeof( char * ) );
	}
  char *exp = envExpansion(argument);

  if (exp) argument = strdup(exp);

  exp = tilde(argument);

  if(exp) argument = strdup(exp);
	
	_arguments[ _numOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numOfArguments + 1] = NULL;
	
	_numOfArguments++;
}
