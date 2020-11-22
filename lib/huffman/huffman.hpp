#ifndef __HUFFMAN_HPP__
#define __HUFFMAN_HPP__
#include <string>
int huffman_compress(std::string inputFile, std::string outputFile);
int huffman_decompress(std::string inputFile, std::string outputFile);
int adapt_huffman_compress(std::string inputFile, std::string outputFile) ;
int adapt_huffman_decompress(std::string inputFile, std::string outputFile);
#endif
