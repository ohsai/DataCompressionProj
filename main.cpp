/* 
 * Compression application using static Huffman coding
 * 
 * Usage: HuffmanCompress InputFile OutputFile
 * Then use the corresponding "HuffmanDecompress" application to recreate the original input file.
 * Note that the application uses an alphabet of 257 symbols - 256 symbols for the byte values
 * and 1 symbol for the EOF marker. The compressed file format starts with a list of 257
 * code lengths, treated as a canonical code, and then followed by the Huffman-coded data.
 * 
 * Copyright (c) Project Nayuki
 * 
 * https://www.nayuki.io/page/reference-huffman-coding
 * https://github.com/nayuki/Reference-Huffman-coding
 */

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <limits>
#include <string>
/*
#include "huffman/BitIoStream.hpp"
#include "huffman/CanonicalCode.hpp"
#include "huffman/FrequencyTable.hpp"
#include "huffman/HuffmanCoder.hpp"
*/
#include "huffman/huffman.hpp"
#include "arithmetic/arithmetic.hpp"

using std::uint32_t;
/*
int arith_compress(std::string inputFile, std::string outputFile){
	std::ifstream in(inputFile.c_str(), std::ios::binary);
	SimpleFrequencyTable freqs(std::vector<uint32_t>(257, 0));
	freqs.increment(256);  // EOF symbol gets a frequency of 1
	while (true) {
		int b = in.get();
		if (b == EOF)
			break;
		if (b < 0 || b > 255)
			throw std::logic_error("Assertion error");
		freqs.increment(static_cast<uint32_t>(b));
	}
	
	// Read input file again, compress with arithmetic coding, and write output file
	in.clear();
	in.seekg(0);
	std::ofstream out(outputFile.c_str(), std::ios::binary);
	BitOutputStream bout(out);
	try {
		
		// Write frequency table
		for (uint32_t i = 0; i < 256; i++) {
			uint32_t freq = freqs.get(i);
			for (int j = 31; j >= 0; j--)
				bout.write(static_cast<int>((freq >> j) & 1));  // Big endian
		}
		
		ArithmeticEncoder enc(32, bout);
		while (true) {
			// Read and encode one byte
			int symbol = in.get();
			if (symbol == EOF)
				break;
			if (symbol < 0 || symbol > 255)
				throw std::logic_error("Assertion error");
			enc.write(freqs, static_cast<uint32_t>(symbol));
		}
		
		enc.write(freqs, 256);  // EOF
		enc.finish();  // Flush remaining code bits
		bout.finish();
		return EXIT_SUCCESS;
		
	} catch (const char *msg) {
		std::cerr << msg << std::endl;
		return EXIT_FAILURE;
	}
}
int arith_decompress(std::string inputFile, std::string outputFile){
	// Perform file decompression
	std::ifstream in(inputFile.c_str(), std::ios::binary);
	std::ofstream out(outputFile.c_str(), std::ios::binary);
	BitInputStream bin(in);
	try {
		
		// Read frequency table
		SimpleFrequencyTable freqs(std::vector<uint32_t>(257, 0));
		for (uint32_t i = 0; i < 256; i++) {
			uint32_t freq = 0;
			for (int j = 0; j < 32; j++)
				freq = (freq << 1) | bin.readNoEof();  // Big endian
			freqs.set(i, freq);
		}
		freqs.increment(256);  // EOF symbol
		
		ArithmeticDecoder dec(32, bin);
		while (true) {
			uint32_t symbol = dec.read(freqs);
			if (symbol == 256)  // EOF symbol
				break;
			int b = static_cast<int>(symbol);
			if (std::numeric_limits<char>::is_signed)
				b -= (b >> 7) << 8;
			out.put(static_cast<char>(b));
		}
		return EXIT_SUCCESS;
		
	} catch (const char *msg) {
		std::cerr << msg << std::endl;
		return EXIT_FAILURE;
	}
}
*/
/*
int compress(std::string inputFile, std::string outputFile){
	
	
	// Read input file once to compute symbol frequencies.
	// The resulting generated code is optimal for static Huffman coding and also canonical.
	std::ifstream in(inputFile.c_str(), std::ios::binary );
	std::ofstream out(outputFile.c_str(), std::ios::binary);
	BitOutputStream bout(out);
	FrequencyTable freqs(std::vector<uint32_t>(257, 0));
	while (true) {
		int b = in.get();
		if (b == EOF)
			break;
		if (b < 0 || b > 255)
			throw std::logic_error("Assertion error");
		freqs.increment(static_cast<uint32_t>(b));
	}
	freqs.increment(256);  // EOF symbol gets a frequency of 1
	CodeTree code = freqs.buildCodeTree();
	const CanonicalCode canonCode(code, freqs.getSymbolLimit());
	// Replace code tree with canonical one. For each symbol,
	// the code value may change but the code length stays the same.
	code = canonCode.toCodeTree();
	
	// Read input file again, compress with Huffman coding, and write output file
	in.clear();
	in.seekg(0);
	try {
		
		// Write code length table
		for (uint32_t i = 0; i < canonCode.getSymbolLimit(); i++) {
			uint32_t val = canonCode.getCodeLength(i);
			// For this file format, we only support codes up to 255 bits long
			if (val >= 256)
				throw std::domain_error("The code for a symbol is too long");
			// Write value as 8 bits in big endian
			for (int j = 7; j >= 0; j--)
				bout.write((val >> j) & 1);
		}
		
		HuffmanEncoder enc(bout);
		enc.codeTree = &code;
		while (true) {
			// Read and encode one byte
			int symbol = in.get();
			if (symbol == EOF)
				break;
			if (symbol < 0 || symbol > 255)
				throw std::logic_error("Assertion error");
			enc.write(static_cast<uint32_t>(symbol));
		}
		enc.write(256);  // EOF
		bout.finish();
		return EXIT_SUCCESS;
		
	} catch (const char *msg) {
		std::cerr << msg << std::endl;
		return EXIT_FAILURE;
	}
}
int decompress(std::string inputFile, std::string outputFile){
	// Perform file decompression
	std::ifstream in(inputFile.c_str(), std::ios::binary);
	std::ofstream out(outputFile.c_str(), std::ios::binary);
	BitInputStream bin(in);
	try {
		
		// Read code length table
		std::vector<uint32_t> codeLengths;
		for (int i = 0; i < 257; i++) {
			// For this file format, we read 8 bits in big endian
			uint32_t val = 0;
			for (int j = 0; j < 8; j++)
				val = (val << 1) | bin.readNoEof();
			codeLengths.push_back(val);
		}
		const CanonicalCode canonCode(codeLengths);
		const CodeTree code = canonCode.toCodeTree();
		
		HuffmanDecoder dec(bin);
		dec.codeTree = &code;
		while (true) {
			uint32_t symbol = dec.read();
			if (symbol == 256)  // EOF symbol
				break;
			int b = static_cast<int>(symbol);
			if (std::numeric_limits<char>::is_signed)
				b -= (b >> 7) << 8;
			out.put(static_cast<char>(b));
		}
		return EXIT_SUCCESS;
		
	} catch (const char *msg) {
		std::cerr << msg << std::endl;
		return EXIT_FAILURE;
	}
}
*/


long fs_b(std::string filename){
	std::ifstream in(filename.c_str(), std::ios::binary );
	long begin = in.tellg();
	in.seekg (0, std::ios::end);
	long end = in.tellg();
	in.close();
	return end-begin;
}

int main(int argc, char *argv[]) {
	// Handle command line arguments
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " InputFile" << std::endl;
		return EXIT_FAILURE;
	}
	std::string comp_ext = ".enc";
	std::string decomp_ext = ".dec";
	
	std::string inputFilename  = argv[1];
	std::string compFilename = (inputFilename + comp_ext);
	std::string decompFilename = (inputFilename + decomp_ext);
	
	std::cout << "input size: " << fs_b(inputFilename) << " bytes." << std::endl;
	
	int status;
	//status = compress(inputFilename, compFilename);
	status = arith_compress(inputFilename, compFilename);
	if(status != EXIT_SUCCESS){
		return status;
	}
	std::cout << "compressed size: " << fs_b(compFilename) << " bytes." << std::endl;
	
	status = arith_decompress(compFilename, decompFilename);
	//status = decompress(compFilename, decompFilename);
	if(status != EXIT_SUCCESS){
		return status;
	}
	std::cout << "decompressed size: " << fs_b(decompFilename) << " bytes." << std::endl;

	if(true){
		std::remove(compFilename.c_str());
		std::remove(decompFilename.c_str());
	}
	//const char *outputFile = argv[2];

}
