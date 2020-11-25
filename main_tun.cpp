#include "tunstall.hpp"
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}
int main (int argc, char* argv[])
{
	signal(SIGSEGV, handler);   // install our handler
	int n;
	try{
	std::string data = "go go gophers my god I am here to save you lmao";
	std::cout << "Enter bit lenght: ";
	std::cin >> n;
	tunstall_tree (data, n);

	return 0;
	}catch( const char* msg){
		std::cerr << msg << std::endl;
		return EXIT_FAILURE;
	}
}
