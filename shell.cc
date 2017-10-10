#include "command.hh"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int yyparse(void);

extern "C" void controlC(int sig) {
  printf("\n");
  Command::_currentCommand.prompt();
}

extern "C" void zombie(int sig) {
  int pid = wait3(0, 0, NULL);
  while (waitpid(-1, NULL, WNOHANG) > 0);
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

  //Zombie killer
  struct sigaction sa2;
  sa2.sa_handler = zombie;
  sigemptyset(&sa2.sa_mask);
  sa2.sa_flags = SA_RESTART;
  error = sigaction(SIGCHLD, &sa2, NULL);
  if (error) {
    perror("sigaction");
    exit(-1);
  }




	Command::_currentCommand.prompt();
	yyparse();
}
