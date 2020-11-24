#ifndef __LZW_HPP__
#define __LZW_HPP__

int lzw_compress(std::string& inputFile, std::string& outputFile, std::string& origin) ;
int lzw_decompress(std::string& inputFile, std::string& outputFile, std::string& uncomp) ;
#endif
