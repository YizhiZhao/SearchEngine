#pragma once

/*
Class Answer
When we search a word or sentence in works of Shakespeare, 
we store all matched answers in a instance of the class Answer.
Doc_num, which records the name of the article which the answer in.
set_num, which records the index of the paragraph in the article.
Rank, which records the rank about this answer. 
We rank all answers according to this attribute.
*/

class Answer
{
private:
	int doc_num;
	int set_num;
	double rank;

public:
	Answer(int, int, double);
	Answer();
	~Answer();
	int get_doc();
	int get_set();
	double get_rank();
	void add_rank(double);
};

