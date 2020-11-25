#include <string>
#include <map>
#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <cmath>
#include "huffman/BitIoStream.hpp"
 
// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
using std::uint32_t;
int lzw_compress(std::string& inputFile, std::string& outputFile, std::string& origin) {
  
  std::ifstream in(inputFile.c_str(), std::ios::binary);
  std::ofstream out(outputFile.c_str(), std::ios::binary);
  BitOutputStream bout(out);
  
	std::string data;
	in.seekg(0, std::ios::end);
	data.reserve(in.tellg());
	in.seekg(0, std::ios::beg);
	data.assign((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());
	//in.read(&data[0], data.size());
	in.close();
  origin = data;
  std::vector<int> compressed;
  std::back_insert_iterator<std::vector<int>> result = std::back_inserter(compressed);
  
  // Build the dictionary.
  int dictSize = 256;
  std::map<std::string,int> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[std::string(1, i)] = i;
 
  std::string w;
  for (std::string::const_iterator it = data.begin();
       it != data.end(); ++it) {
    char c = *it;
    std::string wc = w + c;
    if (dictionary.count(wc))
      w = wc;
    else {
      *result++ = dictionary[w];
      // Add wc to the dictionary.
      dictionary[wc] = dictSize++;
      w = std::string(1, c);
    }
  }
 
  // Output the code for w.
  if (!w.empty())
    *result++ = dictionary[w];

  int bits = std::log2(dictSize) + 1;
    for (int j = 32 - 1; j >= 0; j--){
	bout.write((bits >> j) & 1);
    }

  for ( std::vector<int>::iterator iter = compressed.begin();
		  iter != compressed.end(); 
		  iter++) {
    int k = *iter;
    
    for (int j = bits - 1; j >= 0; j--){
	bout.write((k >> j) & 1);
    }
  }
  bout.finish();

  return EXIT_SUCCESS;
}
 
// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
int lzw_decompress(std::string& inputFile, std::string& outputFile, std::string& uncomp) {
  std::ifstream in(inputFile.c_str(), std::ios::binary);
  std::ofstream out(outputFile.c_str(), std::ios::binary);
  BitInputStream bin(in);

  uint32_t bits = 0;
  for (int j = 0; j < 32; j++){
	bits = (bits << 1) | bin.readNoEof();
  }
  // Build the dictionary.
  int dictSize = 256;
  std::map<int,std::string> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[i] = std::string(1, i);
 
  std::vector<int> compressed;
  std::back_insert_iterator<std::vector<int>> compressed_it = std::back_inserter(compressed);
  bool eof = false;
  while(1){
	  uint32_t k = 0;
	  for (int j = 0; j < static_cast<int>(bits); j++){
		int nextbit = bin.read();
		if(nextbit == -1){
		  eof = true;
		  break;
		}		  
		k = (k << 1) | nextbit ;
	  }
	  if(eof){
		break;
	  }
	  *compressed_it++ = static_cast<int>(k);
  }
  //compressed.pop_back(); // Remove eof

  std::vector<int>::iterator begin = compressed.begin();
  std::string w(1, *begin++);
  std::string result = w;
  std::string entry;
  for (; begin != compressed.end(); begin++) {
    int k = *begin;
    if (dictionary.count(k))
      entry = dictionary[k];
    else if (k == dictSize)
      entry = w + w[0];
    else
      throw "Bad compressed k";
 
    result += entry;
 
    // Add w+entry[0] to the dictionary.
    dictionary[dictSize++] = w + entry[0];
 
    w = entry;
  }
  out << result;
  out.close();
  uncomp = result;
  return EXIT_SUCCESS;
}
 
 /*
int main() {
  std::vector<int> compressed;
  compress("TOBEORNOTTOBEORTOBEORNOT", std::back_inserter(compressed));
  copy(compressed.begin(), compressed.end(), std::ostream_iterator<int>(std::cout, ", "));
  std::cout << std::endl;
  std::string decompressed = decompress(compressed.begin(), compressed.end());
  std::cout << decompressed << std::endl;
 
  return 0;
}
*/

