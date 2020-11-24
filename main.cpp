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
#include <sys/time.h>

#include "huffman/huffman.hpp"
#include "arithmetic/arithmetic.hpp"
#include "lz77.h"
#include "lzw.hpp"
#include <sstream>

struct bm {
    struct timeval b;
    std::string msg;
    bm(const std::string& s) : msg(s) {
        gettimeofday(&b, NULL);
    }
    ~bm() {
        struct timeval e;
        gettimeofday(&e, NULL);
        size_t a = (e.tv_sec*1e6 + e.tv_usec);
        size_t q = (b.tv_sec*1e6 + b.tv_usec);
        std::cout << msg << ": " << ((double)a-(double)q)/1e6 << std::endl;
    }
};

struct bm_s {
    struct timeval b;
    double& _s;
    bm_s(double& s) : _s(s) {
        gettimeofday(&b, NULL);
    }
    ~bm_s() {
        struct timeval e;
        gettimeofday(&e, NULL);
        size_t a = (e.tv_sec*1e6 + e.tv_usec);
        size_t q = (b.tv_sec*1e6 + b.tv_usec);
        _s += ((double)a-(double)q)/1e6;
    }
};


using std::uint32_t;

long fs_b(std::string filename){
	std::ifstream in(filename.c_str(), std::ios::binary );
	long begin = in.tellg();
	in.seekg (0, std::ios::end);
	long end = in.tellg();
	in.close();
	return end-begin;
}
int lz77_compress(std::string inputFile, std::string outputFile, std::string& origin){
	try {
		
		std::ifstream in(inputFile.c_str(),  std::ios::in | std::ios::binary);

		std::string data;
		in.seekg(0, std::ios::end);
		data.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&data[0], data.size());
		in.close();
		origin = data;


		std::string comp;
		lz77::compress_t compress;
		{
		bm _x2("Compression time");
		comp = compress.feed(data);
		}
		std::ofstream out(outputFile.c_str());
		out << comp;
		out.close();
		return EXIT_SUCCESS;
	} catch (const char *msg) {
		std::cerr << msg << std::endl;
		return EXIT_FAILURE;
	}
}
int lz77_decompress(std::string inputFile, std::string outputFile, std::string& uncomp){
	try {
		
		std::ifstream in(inputFile.c_str(),  std::ios::in | std::ios::binary);

		std::string data;
		in.seekg(0, std::ios::end);
		data.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&data[0], data.size());
		in.close();

		std::string decomp;
		lz77::decompress_t decompress;
		{
		bm _x2("Decompression time");
		
		std::string extra;

		if (!decompress.feed(data, extra) || extra.size() > 0) {
		    std::cout << "Sanity error: failed to decompress whole buffer." << std::endl;
		    return 1;
		}
		}
		std::ofstream out(outputFile.c_str());
		out << decompress.result();
		uncomp = decompress.result();
		out.close();
		return EXIT_SUCCESS;
	} catch (const char *msg) {
		std::cerr << msg << std::endl;
		return EXIT_FAILURE;
	}
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
	std::string data;
	std::string uncomp;

	status = lzw_compress(inputFilename, compFilename, data );
	//status = lz77_compress(inputFilename, compFilename,data );
	//status = arith_compress(inputFilename, compFilename);
	//status = compress(inputFilename, compFilename);
	if(status != EXIT_SUCCESS){
		return status;
	}
	std::cout << "compressed size: " << fs_b(compFilename) << " bytes." << std::endl;
	
	status = lzw_decompress(compFilename, decompFilename, uncomp);
	//status = lz77_decompress(compFilename, decompFilename,uncomp);
	//status = arith_decompress(compFilename, decompFilename);
	//status = decompress(compFilename, decompFilename);
	if(status != EXIT_SUCCESS){
		return status;
	}
	std::cout << "decompressed size: " << fs_b(decompFilename) << " bytes." << std::endl;
	
	if (data.size() < 80){
		std::cout << "Content : \n" << data << std::endl;
		std::cout << "Uncomp : \n" << uncomp << std::endl;
	}
	
	if (data != uncomp) {
		std::cout << "Compression-decompression equivalence test failed!" << std::endl;

		return EXIT_FAILURE;
	}


	if(true){
		std::remove(compFilename.c_str());
		std::remove(decompFilename.c_str());
	}
	//const char *outputFile = argv[2];
	//

}
