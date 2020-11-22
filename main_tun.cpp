#include "tunstall.hpp"
#include <iostream>
int main (int argc, char* argv[])
{
	int n;
	std::string data;
	std::cout << "Enter character sequence: ";
	std::cin >> data;
	std::cout << "Enter bit lenght: ";
	std::cin >> n;
	tunstall_tree (data, n);
	return 0;
}
