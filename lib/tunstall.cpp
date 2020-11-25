#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <queue>
#include <cmath>
#include <fstream>
#include "huffman/BitIoStream.hpp"
#include <bitset>

std::vector<int> binary_vec;

class Node
{
public:
	std::string data;
	float prob;
	std::vector<Node*> child;
	Node(std::string data, float prob);
};


Node::Node (std::string data, float prob)
{
	this->data = data;
	this->prob = prob;
}


std::unordered_map<char, int> total_count_per_char (std::string data)
{
	std::unordered_map<char, int> char_counts;

	for (int i = 0; i < data.length(); i++)
	{
		char_counts[data[i]]++;
	}

	return char_counts;
}


std::unordered_map<char, float> prob_per_char (std::string data, std::unordered_map<char, int> char_counts)
{
	std::unordered_map<char, float> char_prob;
	int string_len = data.length ();

	for (auto pair : char_counts)
	{
		float prob = (float) pair.second / (float) string_len;
		char_prob[pair.first] = prob;
	}

	return char_prob;
}


int max_char_idx (std::unordered_map<char, float> probs)
{
	//char ch;
	int idx = 0;
	float max = 0.0;
	int max_idx = 0;
	
	for (auto it = probs.begin (); it != probs.cend (); ++it)
	{
		if (it->second > max)
		{
			max_idx = idx;
			//ch = it->first;
			max = it->second;
		}
	}

	return max_idx;
	//std::string max_char_strg (1, ch);
	//return max_char_strg;
}


void insert_to_vector (std::vector<int> arr, int n)
{
	int value = 0;

	for (int i = 0; i < n; i++)
	{
		value = value << 1;
		value = value | arr[i];
	}

	binary_vec.push_back (value);
}


void generate_binary (int n, std::vector<int> arr, int i)
{
	if (i == n)
	{
		insert_to_vector (arr, n);
		return;
	}

	arr[i] = 0;
	generate_binary (n, arr, i + 1);

	arr[i] = 1;
	generate_binary (n, arr, i + 1);
}


std::map<std::string, int> encode (std::vector<std::string> strings_to_encode)
{
	std::map<std::string, int> encoded_map;

	for (int i = 0; i < strings_to_encode.size (); i++)
	{
		encoded_map.insert ({strings_to_encode[i], binary_vec[i]});
	}

	return encoded_map;
}

void write_bits(BitOutputStream& bout, int value, int bits){
    if(bits <= 1){
    	throw "write_bits bits less or equal to 1";
    }
    if((value >> bits) > 0){
    	throw "write_bits value larger than 2^bits";
    }
    for (int j = bits - 1; j >= 0; j--){
	bout.write((value >> j) & 1);
    }
}


std::tuple<
	std::unordered_map<char,float> , 
	std::map<std::string, int>,
	char
	> 
	tunstall_tree (std::string data, const int n)
{
	// Scan alphabets and their frequency
	std::unordered_map<char, int> char_counts;
	std::unordered_map<char, float> probs;
	char_counts = total_count_per_char (data);
	probs = prob_per_char (data, char_counts);
	int num_alphabets = probs.size();
	std::cout << "alphabets: " << num_alphabets<<std::endl;
	if(num_alphabets > pow(2,n)){
		throw "Number of alphabets cannot be handled with given bit length";
	}

	// Initialize tunstall tree
	Node* root = new Node ("",1.0);
	for (auto pair : probs)
	{
		std::string ch (1, pair.first);
		root->child.push_back( new Node (ch, pair.second));
		std::cout << "char "<<ch << " | prob : " << pair.second<<std::endl;
	}

	// parameters assignment
	int max_idx = max_char_idx (probs);
	std::vector<std::string> strings_to_encode;
	//int iterations = floor (((pow(2, n)) - probs.size ()) / (probs.size () - 1));
	int iterations = floor (((pow(2, n))) / (probs.size ()));
	std::cout << "height: " << iterations<<std::endl;


	// Construct tree
	Node* parent = root;
	Node* max_child = parent->child[max_idx]; 

	for (int step = 0; step < iterations; step++)
	{
		std::vector<Node*> childs = parent->child;

		for (int i = 0; i < childs.size (); i++)
		{
			std::string new_string = "";
			new_string.append (parent->data);
			new_string.append (root->child[i]->data);
			strings_to_encode.push_back (new_string);

			float prob = max_child->prob * root->child[i]->prob;
			max_child->child.push_back (new Node(new_string, prob));
		}

		parent = max_child;
		max_child = parent->child[max_idx]; 
	}

	// Create binary bit mapping table
	std::vector<int> arr(num_alphabets);
	generate_binary (n, arr, 0);
	
	// Map tunstall tree to bit mapping
	std::map<std::string, int> encoded_map = encode (strings_to_encode);

	for (auto pair : encoded_map)
	{
		std::cout << pair.first << "\t\t" <<std::bitset< 64 >( pair.second ) << std::endl;
	}
	
	// Return output tuple
	char max_char_i =  root->child[max_idx]->data[0]; //max_char_strg[0];
	return std::make_tuple(probs, encoded_map, max_char_i);

}

int tunstall_compress( std::string& inputFile, std::string& outputFile, std::string& origin){
	int n, num_alphabets,max_char_i ;
	std::cout << "Enter bit length: ";
	std::cin >> n;
	if( n > 256){
		std::cout << "Error : codelength > 256" << std::endl;
	}
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

	std::tuple<
		std::unordered_map<char,float> , 
		std::map<std::string, int>,
		char
		> output;

	try{
		output = tunstall_tree(data, n);
	}catch(const char* msg){
		std::cerr << msg << std::endl;
		return EXIT_FAILURE;
	
	}
	std::unordered_map<char,float> char_probs = std::get<0>(output);
	std::map<std::string, int> encoded_map = std::get<1>(output);
	max_char_i = std::get<2>(output);

	num_alphabets = char_probs.size();	

	write_bits(bout, n, 8);
	write_bits(bout, num_alphabets, 8);
	write_bits(bout, max_char_i, 8);
	for (auto char_prob : char_probs){
		write_bits(bout, static_cast<int>(char_prob.first), 8);
	}
	for (std::string::const_iterator it = data.begin();
		it != data.end(); ++it) {
		char c = *it;
		std::string key = "" + c;
		while(static_cast<int>(c) == max_char_i){
			++it;
			c = *it;
			key += c;
		}
		int value = encoded_map[key];
		write_bits(bout, value, n);	
	}
	bout.finish();
  	return EXIT_SUCCESS;

}
