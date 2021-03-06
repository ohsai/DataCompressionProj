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
#include "tunstall.hpp"
#include "argparse.hpp"
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
        std::cout << msg << "(us): " << ((double)a-(double)q)/1e6 << std::endl;
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
	argparse::ArgumentParser program("program name");
	program.add_argument("mode")
	  .help("compression mode [lz77/lzw/huff/adahuff/arith/tun]")
	  .action([](const std::string& value) { return value; });
	program.add_argument("inputFile")
	  .help("inputFile path to compress")
	  .action([](const std::string& value) { return value; });
	// Handle command line arguments
	program.add_argument("--num_bits")
	  .help("Number of fixed bits for tunstall")
	  .default_value(8)
	  .action([](const std::string& value) { return std::stoi(value); });

	try {
	  program.parse_args(argc, argv); 
	}
	catch (const std::runtime_error& err) {
	  std::cout << err.what() << std::endl;
	  std::cout << program;
	  exit(0);
	}
	std::string comp_ext = ".enc";
	std::string decomp_ext = ".dec";
	
	std::string inputFilename  =  program.get<std::string>("inputFile"); 
	std::string mode = program.get<std::string>("mode");
	std::string compFilename = (inputFilename+"." + mode + comp_ext);
	std::string decompFilename = (inputFilename + decomp_ext);
	
	std::cout << "input size: " << fs_b(inputFilename) << " bytes." << std::endl;
	
	int status = EXIT_SUCCESS;
	std::string data;
	std::string uncomp;
	{
		bm _x1("Compressing time");
		if(mode == "tun"){
			int num_bits = program.get<int>("--num_bits");
			status = tunstall_compress(inputFilename, compFilename, data, num_bits );
		}	
		if(mode == "lzw"){
			status = lzw_compress(inputFilename, compFilename, data );
		}	
		if(mode == "lz77"){
			status = lz77_compress(inputFilename, compFilename,data );
		}	
		if(mode == "arith"){
			status = arith_compress(inputFilename, compFilename);
		}	
		if(mode == "huff"){
			status = huffman_compress(inputFilename, compFilename);
		}	
		if(mode == "adahuff"){
			status = adapt_huffman_compress(inputFilename, compFilename);
		}	

	}
	if(status != EXIT_SUCCESS){
		return status;
	}
	std::cout << "Compressed size: " << fs_b(compFilename) << " bytes." << std::endl;
	
	{
        	bm _x2("Decompressing time");
		try{
		if(mode == "tun"){
		status = tunstall_decompress(compFilename, decompFilename, uncomp);
		}	
		if(mode == "lzw"){
			status = lzw_decompress(compFilename, decompFilename, uncomp);
		}	
		if(mode == "lz77"){
			status = lz77_decompress(compFilename, decompFilename,uncomp);
		}	
		if(mode == "arith"){
			status = arith_decompress(compFilename, decompFilename);
		}	
		if(mode == "huff"){
			status = huffman_decompress(compFilename, decompFilename);
		}	
		if(mode == "adahuff"){
			status = adapt_huffman_decompress(compFilename, decompFilename);
		}	
		}catch(const char* msg){
		 std::cout << "Error:"<<msg<<std::endl;
		}
	}
	if(status != EXIT_SUCCESS){
		return status;
	}
	std::cout << "Decompressed size: " << fs_b(decompFilename) << " bytes." << std::endl;
	/*
	if (data.size() < 80){
		std::cout << "Content : \n" << data << std::endl;
		std::cout << "Uncomp : \n" << uncomp << std::endl;
	}
	*/
	
	if (data != uncomp) {
		std::cout << "Compression-decompression equivalence test failed!" << std::endl;

		return EXIT_FAILURE;
	}


	if(true){
		//std::remove(compFilename.c_str());
		std::remove(decompFilename.c_str());
	}
	//const char *outputFile = argv[2];
	//

}
