#ifndef __TUNSTALL_HPP__
#define __TUNSTALL_HPP__
#include <string>
void tunstall_tree (std::string data, int n);
int tunstall_compress( std::string& inputFile, std::string& outputFile, std::string& origin);
int tunstall_decompress(std::string inputFile, std::string outputFile, std::string& uncomp);
#endif 
