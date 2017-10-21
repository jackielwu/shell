#ifndef simplcommand_h
#define simplecommand_h

struct SimpleCommand {
  // Available space for arguments currently preallocated
  int _numOfAvailableArguments;

  // Number of arguments
  int _numOfArguments;
  char ** _arguments;

  char *envExpansion(char *argument);
  char *tilde(char *argument);

  SimpleCommand();
  void insertArgument( char * argument );
};

#endif
