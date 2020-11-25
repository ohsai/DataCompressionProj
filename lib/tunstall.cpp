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
std::vector<std::string> binary_vec2;

struct Node
{
	std::string data;
	float prob;
	std::vector<Node*> child;
};


Node* compare (std::vector<Node*> child)
{
	Node* temp = child[0];

	for (int i=1; i<child.size() ; i++)
	{
		if ((child[i]->prob) > (temp->prob))
		{
			temp = child[i];
		}
	}

	return temp;
}


Node* insert_node (std::string data, float prob)
{
	Node* new_node = new Node ();
	new_node->data = data;
	new_node->prob = prob;
	return new_node;
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


std::string max_char (std::unordered_map<char, float> probs)
{
	char ch;
	float max = 0.0;
	
	for (auto it = probs.begin (); it != probs.cend (); ++it)
	{
		if (it->second > max)
		{
			ch = it->first;
			max = it->second;
		}
	}

	std::string max_char_strg (1, ch);
	return max_char_strg;
}


std::vector<std::string> chars_in_arr (std::unordered_map<char, float> probs, std::string max_char_strg)
{
	std::vector<std::string> strings_to_encode;

	for (auto pair : probs)
	{
		std::string new_strg (1, pair.first);
		if (new_strg != max_char_strg)
		{
			strings_to_encode.push_back (new_strg);
		}
	}

	return strings_to_encode;
}


void insert_to_vector (std::vector<int> arr, int n)
{
	std::string binary_string = "";
	int value = 0;

	for (int i = 0; i < n; i++)
	{
		value = value << 1;
		value = value | arr[i];
		binary_string += std::to_string (arr[i]);
	}

	binary_vec.push_back (value);
	binary_vec2.push_back (binary_string);
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

	Node* root = new Node ();
	Node* temp1 = root;
	Node* temp2 = root;
	std::unordered_map<char, int> char_counts;
	std::unordered_map<char, float> probs;

	char_counts = total_count_per_char (data);
	probs = prob_per_char (data, char_counts);

	for (auto pair : probs)
	{
		std::string chat_to_string (1, pair.first);
		root->child.push_back(insert_node (chat_to_string, pair.second));
	}

	std::string max_char_strg = max_char (probs);

	std::vector<std::string> strings_to_encode = chars_in_arr (probs, max_char_strg);

	int iterations = floor (((pow(2, n)) - probs.size ()) / (probs.size () - 1));

	temp1 = compare (temp1->child);

	while (iterations > 0)
	{
		std::vector<Node*> childs = temp2->child;

		for (int i = 0; i < childs.size (); i++)
		{
			std::string new_string = "";
			new_string.append (temp1->data);
			new_string.append (root->child[i]->data);
			strings_to_encode.push_back (new_string);

			float prob = temp1->prob * childs[i]->prob;
			temp1->child.push_back (insert_node (new_string, prob));
		}

		temp2 = temp1;
		temp1 = compare (temp1->child);
		iterations--;
	}

	int size_of_string_vec = strings_to_encode.size ();
	std::vector<int> arr(probs.size());
	generate_binary (n, arr, 0);
	
	std::map<std::string, int> encoded_map = encode (strings_to_encode);

	for (auto pair : encoded_map)
	{
		std::cout << pair.first << "\t\t" <<std::bitset< 64 >( pair.second ) << std::endl;
	}
	
	char max_char_i = max_char_strg[0];
	return std::make_tuple(probs, encoded_map, max_char_i);

	// std::cout << "Highest Probability is: " << temp1->prob << " of " << temp1->data << " character." << std::endl;
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

	auto output = tunstall_tree(data, n);
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
