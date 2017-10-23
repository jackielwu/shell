/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>
#include <pwd.h>

#include "command.hh"


Command::Command()
{
	// Create available space for one simple command
	_numOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numOfSimpleCommands = 0;
	_outFile = 0;
	_inFile = 0;
	_errFile = 0;
	_background = 0;

  _append = 0;
  _inCounter = 0;
  _outCounter = 0;

}
void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
	if ( _numOfAvailableSimpleCommands == _numOfSimpleCommands ) {
		_numOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numOfSimpleCommands ] = simpleCommand;
	_numOfSimpleCommands++;
}

void Command:: clear() {
	for ( int i = 0; i < _numOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inFile ) {
		free( _inFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numOfSimpleCommands = 0;
	_outFile = 0;
	_inFile = 0;
	_errFile = 0;
	_background = 0;
}

void Command::print() {
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inFile?_inFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

int Command::builtInCheck(int i) {
  //Check if builtin command is called
/*  
  if (!strcmp(_simpleCommands[i]->_arguments[0], "source")) {
    int error = source(_simpleCommands[i]->_arguments[1]);
    if (error)
      perror("source");
    clear();
    prompt();
    return 1;
  }
*/
  
  
  if (!strcmp(_simpleCommands[i]->_arguments[0], "setenv")) {
    int error = setenv(_simpleCommands[i]->_arguments[1], _simpleCommands[i]->_arguments[2], 1);
    if (error)
      perror("setenv");
    clear();
    prompt();
    return 1;
  }

  if (!strcmp(_simpleCommands[i]->_arguments[0], "unsetenv")) {
    int error = unsetenv(_simpleCommands[i]->_arguments[1]);
    if (error)
      perror("unsetenv");
    clear();
    prompt();
    return 1;
  }

  if (!strcmp(_simpleCommands[i]->_arguments[0], "cd")) {
    int error;
    if (_simpleCommands[i]->_numOfArguments == 1) {
      error = chdir(getenv("HOME"));
    }
    else {
      error = chdir(_simpleCommands[i]->_arguments[1]);
    }

    if (error < 0) {
      perror("chdir");
    }

    clear();
    prompt();
    return 1;
  }
  return 0;
}

void Command::execute() {
	//_outCounter =1;
  // Don't do anything if there are no simple commands
	if ( _numOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

  if (!strcmp(_simpleCommands[0]->_arguments[0], "exit")) {
    printf("Good bye!!\n");
    exit(1);
  }

  if (_inCounter > 1 || _outCounter >= 2) {
    printf("Ambiguous output redirect.\n");
    clear();
    prompt();
    return;
  }

	// Print contents of Command data structure
	//print();

	// Add execution here
	int tmpin = dup(0);
  int tmpout = dup(1);
  int tmperr = dup(2);
  
  int fdin;
  int fdout;
  int fderr;
  int ret;
  
  if (_inFile) {
    fdin = open(_inFile, O_RDONLY);
  } else {
    fdin = dup(tmpin);
  }

  if (_errFile) {
    if (_append) {
      fderr = open(_errFile, O_WRONLY | O_CREAT | O_APPEND, 0660);
    } else {
      fderr = open(_errFile, O_WRONLY | O_CREAT | O_APPEND, 0660);
    }
  } else {
    fderr = dup(tmperr);
  }
  dup2(fderr, 2);
  close(fderr);

  // For every simple command fork a new process
	for ( int i = 0; i < _numOfSimpleCommands; i++) {
    
    dup2(fdin, 0);
    close(fdin);
    
    //Check for builtins
    if (builtInCheck(i)) return;

    if (i == _numOfSimpleCommands - 1) {
      if (_outFile) {
        if (_append) {
          fdout = open(_outFile, O_WRONLY | O_CREAT | O_APPEND, 0660);
        } else {
          fdout = open(_outFile, O_WRONLY | O_CREAT | O_TRUNC, 0660);
        } 
      } else {
        fdout = dup(tmpout);
      }
    } else {
      int fdpipe[2];
      pipe(fdpipe);

      fdout = fdpipe[1];
      fdin = fdpipe[0];
    }
    
    dup2(fdout, 1);
    close(fdout);

    ret = fork();
    if ( ret == 0) {
      //child
      
      if (!strcmp(_simpleCommands[i]->_arguments[0], "printenv")) {
        char **env = environ;
        while (*env) {
          printf("%s\n", *env);
          env++;
        }
      }

      execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);
      perror("execvp");
      _exit(1);
    }
    else if (ret < 0) {
      perror("fork");
      return;
    }
    //Parent shell continue
  }

  dup2(tmpin, 0);
  dup2(tmpout, 1);
  dup2(tmperr, 2);
  close(tmpin);
  close(tmpout);
  close(tmperr);

  if (!_background) {
    waitpid(ret, NULL, 0);
  }
  // Setup i/o redirection
	// and call exec

	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation

void Command::prompt() {
	if (isatty(0)) {
    printf("myshell>");
	  fflush(stdout);
  }
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;
