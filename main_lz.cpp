
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "lz.hpp"
using namespace std;
int main()
{
	string input, result, method_text;
	int method, option, option2;

	string intro = R"(
	Hi I'm Alpha  ^_^ , Code Expo's assistant, I'm here to help you.
)";
	cout << intro;
main_menu:
	string main_menu = R"(
-------------------------------------------------------------------------------
 This tool generate compression and decompression using LZ-77, LZ-78 and LZW methods :

   1- LZ-77
   2- LZ-78
   3- LZW

 Enter 1, 2 or 3 according to method.
  > )";	cout << main_menu;

	cin >> method;

	if (method == 1)
		method_text = "LZ-77";
	else if (method == 2)
		method_text = "LZ-78";
	else if (method == 3)
		method_text = "LZW";
	else
	{
		system("cls");
		cout << intro;
		goto main_menu;
	}

method_menu:
	system("cls");
	cout << intro;

	string main_menu_2 = R"(
-------------------------------------------------------------------------------
 This tool generate compression and decompression using )" + method_text + R"( method :

   1- Compression
   2- Decompression

   0- Main menu

 Enter 1, 2 or 0 according to method.
  > )";	cout << main_menu_2;

	cin >> option;

	if (option == 1)
	{
		system("cls");
		cout << intro;

		string lz77_Compression = R"(
-------------------------------------------------------------------------------
 )" + method_text + R"( >  Compression :)";
		cout << lz77_Compression << endl;

		cout << "\t Enter your phrases : ";
		cin.ignore();
		getline(cin, input);
		if (method == 1)
			result = LZ77(input, 1);
		else if (method == 2)
			result = LZ78(input, 1);
		else if (method == 3)
			result = LZW(input, 1);
		else
		{
			system("cls");
			cout << intro;
			goto main_menu;
		}

		cout << "\n\t Final result : " << result << endl;

	back_1:
		cout << "\n Enter 0 to back to Main menu or 1 to back to Method menu. \n  > ";
		cin >> option2;

		if (option2 == 0)
		{
			system("cls");
			cout << intro;
			goto main_menu;
		}
		else if (option2 == 1)
			goto method_menu;
		else
			goto back_1;
	}
	else if (option == 2)
	{
		system("cls");
		cout << intro;

		string lz77_Compression = R"(
-------------------------------------------------------------------------------
 LZ-77 >  Decompression :)";
		cout << lz77_Compression << endl;
		//cout << "Note : Enter 0 for NULL characters\n\n";
		cout << "\t Enter your code : ";
		cin.ignore();
		getline(cin, input);
		if (method == 1)
			result = LZ77(input, 2);
		else if (method == 2)
			result = LZ78(input, 2);
		else if (method == 3)
			result = LZW(input, 2);
		else
			main_menu;

		cout << "\n\t Final result : " << result << endl;

	back_2:
		cout << "\n Enter 0 to back to Main menu or 1 to back to Method menu. \n  > ";
		cin >> option2;

		if (option2 == 0)
		{
			system("cls");
			cout << intro;
			goto main_menu;
		}
		else if (option2 == 1)
			goto method_menu;
		else
			goto back_2;

	}
	else if (option == 0)
	{
		system("cls");
		cout << intro;
		goto main_menu;
	}
	else
		goto method_menu;


	cin.get();
	cin.ignore();
	return 0;
}


