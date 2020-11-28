//#define _CRT_SECURE_NO_DEPRECATE
//#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include "stdlib.h"
#include <time.h>
#include "golomb.hpp"
#include <fstream>
#include <string>
/*
#define p 0.9655
#define m 9
#define b (int)ceil(logf((double)m)/logf(2.0))
#define c (int)pow(2.0,(double)b)-m
#define size 10000
*/

//float p = 0.9655;
float p = 0.2;
int m = 9;
int b = (int)ceil(logf((double)m)/logf(2.0));
int c = (int)pow(2.0,(double)b)-m;
const int size =  10000;
double Ravn() { // равномерное распределение
	double result = (double)(rand()) / RAND_MAX;
	return result;
}

double Geom(double u) { // геометрическое распределение
	int cnt = 1;
	double q = Ravn();
	while (q > u) {
		cnt++;
		q = Ravn();
	}
	return (double)cnt;
}

double Statistics(int mas[size]) { // построение гистограмм, рассчет теоретической энтропии
	FILE *out = fopen("geometric_statistics.xls", "wb+" );
	int arr[size] = { 0 };
	double entropy = 0;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < size;j++)
			if (mas[j] == i)
				arr[i]++;
	for (int i = 0; i < 100; i++) {
		fprintf(out, "%d\t%d\n", i, arr[i]);
	}
	fclose(out);
	for (int i = 0; i < 100; i++) {
		if(arr[i]!=0)
			entropy += -((double)arr[i] / size)*logf((double)arr[i] / size);
	}
	return entropy;
}

int Check(FILE *in1, FILE * in2) { // сравнение входа кодера с выходом декодера
	char ch1, ch2;
	int cnt = 0;
	ch1 = getc(in1);
	ch2 = getc(in2);
	while ((ch1 != EOF||ch2 != EOF)&& cnt<size) {
		if (ch1 == ch2) {
			ch1 = getc(in1);
			ch2 = getc(in2);
			cnt++;
		}
		else
			return 0;
	}
	return 1;
}

int** table(int use_m, int use_b, int use_c) { // построение таблицы остатков для кода Голомба
	printf("p = %f\n", p);
	printf("m = %d\n", use_m);
	printf("b = %d\n", use_b);
	printf("c = %d\n", use_c);
	int temp = 0;
	int **mas = new int*[use_m];
	for (int i = 0; i < use_m; i++)
		mas[i] = new int[use_b];
	for (int k = 0; k < use_m; k++) {
		temp = k + use_c;
		if (k < use_c)
			temp = k;
		for (int q = 0; q < use_b + 1; q++) {
			if (temp % 2 == 0)
				mas[k][use_b - 1 - q] = 0;
			else
				mas[k][use_b - 1 - q] = 1;
			temp = temp / 2;
		}
	}/*
	for (int i = 0; i < use_m; i++)
		for (int j = 0; j < use_b; j++) {
			printf("%d", mas[i][j]);
			if (j == use_b - 1)
				printf("\n");
		}*/
	return mas;
}

int getM(double u) { // подбор оптимального значения параметра m
	int new_m = 1;
	double pr = double(1) / (u + 1);
	bool flag = false;
	double l_board = 0;
	double r_board = 0;
	while (true) {
		l_board = pow(u, (double)new_m);
		r_board = pow(u, (double)(new_m - 1));
		if (pr >= l_board && pr < r_board) {
			flag = true;
			break;
		}
		else {
			new_m++;
		}
	}
	return new_m;
}

int getB(int new_m) { // вычисление значения параметра b
	int new_b = (int)ceil(logf((double)new_m) / logf(2.0));
	return new_b;
}

int getC(int new_m, int new_b) { // вычисление значения параметра c
	int new_c = (int)pow(2.0, (double)new_b) - new_m;
	return new_c;
}

void coder_gol(int *n, int **mas, FILE *out, FILE *out1, int use_m, int use_b, int use_c) { // кодер кода Голомба
	int q, r;
	int count = 0;
	char buf = 0;
	for (int i = 0; i < size; i++) {
		if(out != NULL){
			fprintf(out, "%d", n[i]);
		}
		q = (int)floor(n[i] / use_m);
		for (int i = 0; i < q; i++) {
			buf = buf | 0 << (7 - count);
			count++;
			if (count == 8) {
				count = 0;
				fprintf(out1, "%c", buf);
				buf = 0;
			}

		}
		buf = buf | 1 << (7 - count);
		count++;
		if (count == 8) {
			count = 0;
			fprintf(out1, "%c", buf);
			buf = 0;
		}
		if (use_m != 1) {
			r = n[i]%use_m;
			if (r < use_c) {
				for (int i = 1; i < use_b; i++) {
					buf = buf | mas[r][i] << (7 - count);
					count++;
					if (count == 8) {
						count = 0;
						fprintf(out1, "%c", buf);
						buf = 0;
					}
				}
			}
			else
				for (int i = 0; i < use_b; i++) {
					buf = buf | mas[r][i] << (7 - count);
					count++;
					if (count == 8) {
						count = 0;
						fprintf(out1, "%c", buf);
						buf = 0;
					}
				}
		}
	}
	if(count<8)
		fprintf(out1, "%c", buf);
}

int Bitt(bool a) { 
	if (a)
		return 1;
	else
		return 0;
}

void decoder_gol(FILE *in, FILE *out, int use_m, int use_b, int use_c) {  //декодер кода Голомба
	int a = 0, r = 0, n = 0, j=0, global_count=0;
	int tmp = (int)use_c;
	int count = 0;
	char ch;
	bool bit;
	int byt;
	ch = getc(in);
	while (global_count < size) {
		a = r = n = 0;
		// вычисляем А
		 bit = ch&(1<<(7-count));
		count++;
		if (count == 8) {
			count = 0;
			ch = getc(in);
		}
		for (int i = 0; (int)bit != 1; i++) {
			a++;
			 bit = ch&(1 << (7 - count));
			count++;
			if (count == 8) {
				count = 0;
				ch = getc(in);
			}
		}
		if (use_m != 1) {
			int *masl = new int[use_b - 1];
			int *masg = new int[use_b];
			// вычисляем R			
			for (int i = use_b - 2; i >= 0; i--) {
				 bit = ch&(1 << (7 - count));
				count++;
				if (count == 8) {
					count = 0;
					ch = getc(in);
				}
				masl[i] = (int)bit;
			}
			for (int i = 0; i < use_b - 1; i++)
				r += (masl[i])*pow(2.0, i);
			if (r < use_c) {
				n = use_m*a + r;
			}
			else {
				r = 0;
				for (int i = 1, j = 0; i < use_b; i++, j++)
					masg[i] = masl[j];
				 bit = ch&(1 << (7 - count));
				count++;
				if (count == 8) {
					count = 0;
					ch = getc(in);
				}
				masg[0] = (int)bit;
				for (int i = 0; i < use_b; i++)
					r += (masg[i])*pow(2.0, i);
				n = use_m*a + r - tmp;
			}
			delete[] masl;
			delete[] masg;
		}
		else
			n = a;
		fprintf(out, "%d", n);
		global_count++;
	}
}

double Entropy(FILE *out1){ // вычисление практического значения энтропии
	char ch;
	double cnt;
	double entropy = 0;
	cnt = 0;
	ch = getc(out1);
	while (ch != EOF) {
		cnt++;
		ch = getc(out1);
	}
	entropy = cnt*8 / size;
	return entropy;
}
int gol_compress(std::string inputFile, std::string outputFile, std::string& origin){
	std::ifstream in(inputFile.c_str(),  std::ios::in | std::ios::binary);

	std::string data;
	in.seekg(0, std::ios::end);
	data.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&data[0], data.size());
	in.close();
	origin = data;


	FILE *out = NULL;
	FILE *out1 = fopen(outputFile.c_str(), "wb+");
	int *arr = new int[data.size()];
	int **mas = new int*[m];
	for (int i = 0; i < m; i++)
		mas[i] = new int[b];
	mas = table(m, b, c);
	for (int i = 0; i < size; i++) {
		arr[i] = static_cast<int>(data[i]);
	}
	arr[0] = 0;

	coder_gol(arr, mas, out, out1, m, b, c);
	fclose(out);
	fclose(out1);
	return EXIT_SUCCESS;
}
int gol_decompress(std::string inputFile, std::string outputFile, std::string& uncomp){
	FILE *out1 = fopen(inputFile.c_str(), "rb+");
	FILE *out2 = fopen(outputFile.c_str(), "wb+");
	decoder_gol(out1, out2, m, b, c);
	uncomp = "";
	fclose(out1);
	fclose(out2);
	return EXIT_SUCCESS;
}

void GolombsCode(int use_m, int use_b, int use_c){
	FILE *out = fopen("Golomb_codes_input_of_coder.txt", "wb+");
	FILE *out1 = fopen("Golomb_codes_out_of_coder.txt", "wb+");
	FILE *out2 = fopen("Golomb_codes_out_of_decoder.txt", "wb+");
	double entropy=0;
	int check = 0;
	int *arr = new int[size];
	int **mas = new int*[use_m];
	for (int i = 0; i < size; i++) {
		arr[i] = 0;
	}
	for (int i = 0; i < use_m; i++)
		mas[i] = new int[use_b];
	mas = table(use_m, use_b, use_c);
	for (int i = 0; i < size; i++) {
		arr[i] = (int)Geom(p);
	}
	arr[0] = 0;
	entropy=Statistics(arr);
	printf("Entropy is %f\n", entropy);
	coder_gol(arr, mas, out, out1, use_m, use_b, use_c);
	fclose(out1);
	out1 = fopen("Golomb_codes_out_of_coder.txt", "rb+");
	decoder_gol(out1, out2, use_m, use_b, use_c);
	fclose(out);
	fclose(out2);
	out = fopen("Golomb_codes_input_of_coder.txt", "rb+");
	out2 = fopen("Golomb_codes_out_of_decoder.txt", "rb+");
	check = Check(out, out2);
	if (check)
		printf("Golomb codes: Files match.\n");
	else
		printf("Golomb codes: Files don't match.\n");
	fclose(out);
	fclose(out1);
	fclose(out2);
	out1 = fopen("Golomb_codes_out_of_coder.txt", "rb+");
	entropy = Entropy(out1);
	printf("Practical entropy is %f\n", entropy);
	fclose(out1);
}


