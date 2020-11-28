#ifndef __GOLOMB_HPP__
#define __GOLOMB_HPP__

#include <math.h>
#include <string>
extern float p;
extern int m;
extern int b;
extern int c;
extern const int size;

int getM(double u) ;
int getB(int new_m); 
int getC(int new_m, int new_b) ;
void GolombsCode(int use_m, int use_b, int use_c);

int gol_compress(std::string inputFile, std::string outputFile, std::string& origin);
int gol_decompress(std::string inputFile, std::string outputFile, std::string& uncomp);
#endif
