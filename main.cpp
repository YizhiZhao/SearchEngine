/*
Step 1 input the word or sentence users want to search. Terminate the program if users input “END”.
Step2 ignore all punctuations and split the sentence into words
Step2 remove the stop words. If no word remained, raise error.
Step4 stem remained words and add into keywords list
Step3 search the inverted index table to find the document number and paragraph number and repeat times of every word in keyword list and store these information in an answer set.
Step4 calculate all answers’ rank and sort them
Step5 print the first three answers, which is a certain paragraph in a certain article, according to the value of rank.

*/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <algorithm>   
#include <cctype>
#include <Windows.h>
#include <vector>
#include "stemWords.h"
#include "Offset.h"
#include "Answer.h"
using namespace std;

#define table_length 40000 //Capacity of Hash table 
#define max_name_length 30 //The max length a book's name
#define doc_sum 44 //The total number of books
#define max_display 3 //The max number of search results displayed 

int  BKDRhash(string key);
string remove_space(string word);
int cal_offset(string word);
double cal_rank(int rep);
void ans_sort();
int is_exist(int new_doc, int new_set);
void my_push_back(int new_doc, int new_set, double new_rank);
int is_stop(string word);
void generate_offset_table();
void generate_doc_name_list();
int is_pun(char temp_char);
void add_word(string temp_word);

Offset * offset_table[table_length];//table that records the offset table of each word
vector<string> word_list;//list of words user search 
vector<Answer> ans_list;//list of research results
string doc_name_list[doc_sum];//list of names of books

int main()
{
	std::cout << "Start Search Engine" << endl << endl;
	generate_offset_table();//Read the offset table form "offset.txt"
	generate_doc_name_list();//Read the list of books' name from "MARKED.txt"
	std::cout << "Now Search Engine is Running" << endl;
	std::cout << "You can Input 'END' to End this program" << endl << endl;
	
	ifstream get_ans;
	get_ans.open("hash.txt");//get inverted index table

	double new_rank;
	char interval;
	string temp_word, word, doc_name, set_name, temp_set_num, sentence, temp_line;
	int i, j, offset, list_length, word_list_length, new_doc, new_set, 
		new_rep, ans_list_length, line_length;

	while (1) //user input
	{
		word_list.clear();
		ans_list.clear();

		getline(cin, temp_line);
		if (temp_line == "END") break;

		line_length = temp_line.size();
		for (i = 0, temp_word = ""; i < line_length; i++)
		{
			if (!is_pun(temp_line.at(i)))
			{
				//cout << i << temp_word << endl;
				temp_word += temp_line.at(i);
			}
			else 
			{
				//cout << i << temp_word << endl;
				add_word(temp_word);
				temp_word = "";
			}
		}
		add_word(temp_word);

		word_list_length = word_list.size();
		if (!word_list_length)
		{
			std::cout << endl << "Please Input Valid Information" << endl << endl;
			continue;
		}

		for (j = 0; j < word_list_length; j++)
		{
			//cout << word_list[j] << endl;
			offset = cal_offset(word_list[j]);
			if (offset == -1) continue;
			get_ans.seekg(offset, ios::beg);
			
			get_ans >> temp_word;
			get_ans >> list_length;

			for (i = 0; i < list_length; i++)
			{
				get_ans >> new_doc >> new_set >> new_rep >> interval;
				new_rank = cal_rank(new_rep);
				my_push_back(new_doc, new_set, new_rank);
			}
		}

		ans_list_length = ans_list.size();
		if (ans_list_length == 0)
		{
			std::cout << endl << "No Answer Found" << endl << endl;
			continue;
		}
		ans_sort();

		if (ans_list_length > max_display) ans_list_length = max_display;
		std::cout << endl << "-------------------------------------------" << endl;
		for (i = 0; i < ans_list_length; i++)
		{
			doc_name = doc_name_list[ans_list[i].get_doc()];	
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
			//change the color of the output
			std::cout << "<<" << doc_name << ">>" << endl << endl; //output the name of a book
			//change the color of the output
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << "Rank " << ans_list[i].get_rank() << endl << endl;
			ifstream get_set;
			temp_set_num = to_string(ans_list[i].get_set());
			set_name = doc_name + "/" + "para_" + temp_set_num + ".txt";

			get_set.open(set_name);
			getline(get_set, sentence); //read in the paragraph
			
			std::cout << "Paragraph " << temp_set_num << endl << endl << sentence << endl; // output the paragraph
			std::cout << endl << "-------------------------------------------" << endl;
		}
	}

	get_ans.close();
	return 0;
}

int BKDRhash(string key)
{
	unsigned int seed = 131, result = 0;
	int i, length = key.length();
	for (i = 0; i < length; i++) result = result * seed + key.at(i);
	result = result & 0x7FFFFFFF;
	result = (int)result;
	return (result % table_length);
}

/*
remove_space()
remove the first space of a string
*/

string remove_space(string word)
{
	char temp[max_name_length];
	int i = 0, length = word.size();
	for (; i < length - 1; i++)
	{
		temp[i] = word.at(i + 1);
	}
	temp[i] = 0;
	string new_word = temp;
	return new_word;
}

/*
ans_sort()
This function use a modified version of insertion sorting to sort the all the research result.
Step 1 Find a search result with rank > 1
	Step 2 Temporarily store this result and delete it in the search result list
	Step3 Find a suitable position of this result and insert it into this postion
Step 4 Back to Step 1

This method is appropriate for this case because almost all the research results in the list only have rank = 1, and we don’t need to move them with this function.

*/

void ans_sort()
{
	int i, j, length = ans_list.size();
	Answer temp_ans;
	for (i = 1; i < length; i++)
	{
		if (ans_list[i].get_rank() > 1) 
		{
			temp_ans = ans_list[i];
			ans_list.erase(ans_list.begin() + i);
			for (j = i - 1; j >= 0; j--) 
			{
				if (ans_list[j].get_rank() >= temp_ans.get_rank())
				{
					ans_list.insert(ans_list.begin() + j + 1, temp_ans);
					break;
				}
			}
			if(j == -1) ans_list.insert(ans_list.begin(), temp_ans);
		}
	}
}

/*
 cal_offset()
This function obtains the offset value of between the beginning and a word in the “hash.txt”.
Step 1 call BRDRhash() to compute the possible position of this word in the offset table
Step 2 judge if this position is exactly the position we want to find
	Step 3 if it is true, return this the position of this Reference
	Step 5 else use linear solution to solve collision and then back to Step 2
Step 4 if the whole offset table is search but no result found, then there is no research result of this word.
*/

int cal_offset(string word)
{
	int index = BKDRhash(word);
	int i, offset = -1;
	for (i = 0; i < table_length; i++)
	{
		if (offset_table[index]->get_key() == word)
		{
			offset = offset_table[index]->get_value();
			break;
		}
		else index = (index + 1) % table_length;
	}
	return offset;
}

/*
cal_rank()
This function turn the rep_num of search result into rank.
Step 1 If the user only search one single word, then its rank equals its rep_num.
	else its rank is 1
*/

double cal_rank(int rep)
{
	double rank;
	if (word_list.size() > 1) rank = 1;
	else rank = rep;
	return rank;
}

/*
my_push_back()
This function is called whenever a new search result is found.
Step 1 Use is_exist() to judge if a search result has been in the search result list.
Step 2 If it is, then add 1 to the rank of this search result
	  else add this new result into the result list
*/

void my_push_back(int new_doc, int new_set, double new_rank)
{
	int index = is_exist(new_doc, new_set);
	if (index != -1) ans_list[index].add_rank(new_rank);
	else 
	{
		Answer new_ans(new_doc, new_set, new_rank);
		ans_list.push_back(new_ans);
	}
	return;
}

/*
is_exist()
Step 1 If there is a search result having the same doc_num and set_num with the new result, then return the order number of this result
	else return -1
*/

int is_exist(int new_doc, int new_set)
{
	int i, index = -1, length = ans_list.size();
	for (i = 0; i < length; i++) if (ans_list[i].get_doc() == new_doc && ans_list[i].get_set() == new_set) index = i;
	return index;
}

/*
is_stop()
judge if a word is a stop word
*/

int is_stop(string word)
{
	if (word == "i" || word == "you" || word == "it" || word == "he" || word == "she"
		|| word == "they" || word == "we" || word == "thou"
		|| word == "my" || word == "his" || word == "her" || word == "its"
		|| word == "their" || word == "our" || word == "your"
		|| word == "me" || word == "him" || word == "them" || word == "us"
		|| word == "is" || word == "are" || word == "was" || word == "were" || word == "am"
		|| word == "be" || word == "been" || word == "being"
		|| word == "will" || word == "has" || word == "have" || word == "had" || word == "hath"
		|| word == "a" || word == "an" || word == "the"
		|| word == "that" || word == "this" || word == "those" || word == "these"
		|| word == "and" || word == "or" || word == "but" || word == "so"
		|| word == "in" || word == "of" || word == "for" || word == "to"
		|| word == "by" || word == "as" || word == "from" || word == "with"
		|| word == "no" || word == "not" || word == "any" || word == "some"
		) return 1;
	else return 0;
}

/*
generate_offset_table()
Read the offset table form "offset.txt"
*/

void generate_offset_table()
{
	ifstream get_offset;
	get_offset.open("offset.txt");
	int i;
	for (i = 0; i < table_length; i++)
	{
		string new_key;
		int new_value;
		get_offset >> new_key >> new_value;

		Offset * new_offset = new Offset(new_key, new_value);;
		offset_table[i] = new_offset;
	}
	get_offset.close();
	return;
}

/*
generate_doc_name_list()
/Read the list of books' name from "MARKED.txt"
*/

void generate_doc_name_list()
{
	ifstream get_doc_name;
	get_doc_name.open("MARKED.txt");
	int i, temp_index;
	char temp_sign;
	string temp_name;
	for (i = 1; i < doc_sum; i++)
	{
		get_doc_name >> temp_index >> temp_sign;
		getline(get_doc_name, temp_name);
		doc_name_list[i] = remove_space(temp_name);//移除文件名首的空格
	}
	get_doc_name.close();
	return;
}

/*
is_pun()
This funtion judge if is a char type variable is a punctuation.
*/

int is_pun(char temp_char)
{
	if (temp_char == ' ' || temp_char == ',' || temp_char == '.' || temp_char == '(' || temp_char == ')'
		|| temp_char == '[' || temp_char == ']' || temp_char == '{' || temp_char == '}'
		|| temp_char == '!' || temp_char == '?' || temp_char == ':' || temp_char == ';')
		return 1;
	else return 0;
}

/*
Add_word()
This function is used to process the word users input and add the word to a certain word list if necessary.
Step1 stem the word to its root if the word is not empty.
Step2 lower every char in this word
Step3 return NULL if the word is a stop word or add it into the specified word list if not.
*/

void add_word(string temp_word)
{
	if (temp_word == "") return;
	string word = temp_word;
	word = stemWords(temp_word);
	transform(word.begin(), word.end(), word.begin(), tolower);
	if (is_stop(word)) return;
	word_list.push_back(word);
	return;
}