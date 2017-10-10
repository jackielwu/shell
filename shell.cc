#include "command.hh"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>


int yyparse(void);

extern "C" void controlC(int sig) {
  printf("\n");
  Command::_currentCommand.prompt();
}









int main() {

  int error;

  //Control-c
  struct sigaction sa1;
  sa1.sa_handler = controlC;
  sigemptyset(&sa1.sa_mask);
  sa1.sa_flags = SA_RESTART;
  error = sigaction(SIGINT, &sa1, NULL);
  if (error) {
    perror("sigaction");
    exit(-1);
  }






	Command::_currentCommand.prompt();
	yyparse();
}
