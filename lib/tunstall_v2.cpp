#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <queue>
#include <cmath>

std::vector<std::string> binary_vec;

struct Node
{
	std::string data;
	float prob;
	std::vector<Node*> child;
};


Node* compare (std::vector<Node*> child)
{
	Node* max = child[0];

		std::cout<<"("<<child[0]->data<<"," << child[0]->prob << ") ";
	for (int i=1; i<child.size() ; i++)
	{
		std::cout<<"("<<child[i]->data<<"," << child[i]->prob << ") ";
		if ((child[i]->prob) > (max->prob))
		{
			max = child[i];
		}
	}
	std::cout << std::endl;

	return max;
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


int max_char_idx (std::unordered_map<char, float> probs)
{
	int idx = 0;
	float max = 0.0;
	int max_idx = 0;
	
	for (auto it = probs.begin (); it != probs.cend (); ++it)
	{
		if (it->second > max)
		{

			max_idx = idx;
			//idx = it - probs.begin();
			max = it->second;
		}
		idx++;
	}

	return max_idx;
	//std::string max_char_strg (1, ch);
	//return max_char_strg;
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
	//std::string binary_string = "";
	int value = 0;

	for (int i = 0; i < n; i++)
	{
		value = value << 1;
		value = value | arr[i];
		//binary_string += std::to_string (arr[i]);
	}

	binary_vec.push_back (value);
	//binary_vec.push_back (binary_string);
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


std::map<std::string, std::string> encode (std::vector<std::string> strings_to_encode)
{
	std::map<std::string, std::string> encoded_map;

	std::cout << "phrases: " << strings_to_encode.size ()<<std::endl;
	std::cout << "binarys: " << binary_vec.size ()<<std::endl;
	for (int i = 0; i < strings_to_encode.size (); i++)
	{
		std::cout <<"str"<<strings_to_encode[i] << std::endl;
		std::cout << "bin" <<binary_vec[i]<< std::endl;
		encoded_map.insert ({strings_to_encode[i], binary_vec[i]});
	}
	std::cout << "b2" << std::endl;

	return encoded_map;
}


void tunstall_tree (std::string data, int n)
{
	Node* root = new Node ();
	Node* parent = root;
	Node* max_child = root;
	std::unordered_map<char, int> char_counts;
	std::unordered_map<char, float> probs;

	char_counts = total_count_per_char (data);
	probs = prob_per_char (data, char_counts);
	std::cout << "alphabets: " << probs.size ()<<std::endl;

	for (auto pair : probs)
	{
		std::string char_to_string (1, pair.first);
		root->child.push_back(insert_node (char_to_string, pair.second));
		std::cout << "char "<<char_to_string << " | prob : " << pair.second<<std::endl;
	}

	int max_idx = max_char_idx (probs);
	std::vector<std::string> strings_to_encode;
	//int iterations = floor (((pow(2, n)) - probs.size ()) / (probs.size () - 1));
	int iterations = floor (((pow(2, n))) / (probs.size ()));
	std::cout << "height: " << iterations<<std::endl;

	max_child = parent->child[max_idx]; //compare (parent->child);

	for (; iterations > 0; iterations--)
	{
		std::vector<Node*> childs = parent->child;

		for (int i = 0; i < childs.size (); i++)
		{
			std::string new_string = "";
			new_string.append (max_child->data);
			new_string.append (root->child[i]->data);
			strings_to_encode.push_back (new_string);

			float prob = max_child->prob * root->child[i]->prob;
			max_child->child.push_back (insert_node (new_string, prob));
		}

		parent = max_child;
		std::cout <<"parent data : " << parent->data << std::endl;
		max_child = parent->child[max_idx]; //compare (parent->child);
		std::cout <<"max_child data : " << max_child->data << std::endl;
	}

	int size_of_string_vec = strings_to_encode.size ();
	std::vector<int> arr(probs.size());
	generate_binary (n, arr, 0);
	
	std::map<std::string, std::string> encoded_map = encode (strings_to_encode);

	for (auto pair : encoded_map)
	{
		std::cout << pair.first << "\t\t" << pair.second << std::endl;
	}
	std::cout << "a3" << std::endl;

	std::cout << "Binary string sequence: ";

	for (auto str : encoded_map)
	{
		std::cout << str.second << "";
	}

	std::cout << std::endl;
	// std::cout << "Highest Probability is: " << max_child->prob << " of " << max_child->data << " character." << std::endl;
}


int main (int argc, char* argv[])
{
	int n;
	std::string data;
	std::cout << "Enter character sequence: ";
	std::cin >> data;
	std::cout << "Enter bit lenght: ";
	std::cin >> n;
	tunstall_tree (data, n);
	return 0;
}
