#include "stdafx.h"
#include "Answer.h"


Answer::Answer(int new_doc, int new_set, double new_rank)
{
	doc_num = new_doc;
	set_num = new_set;
	rank = new_rank;
}

Answer::Answer()
{
	doc_num = -1;
	set_num = -1;
	rank = -1;
}


Answer::~Answer()
{
}

int Answer::get_doc()
{
	return doc_num;
}

int Answer::get_set()
{
	return set_num;
}

double Answer::get_rank()
{
	return rank;
}

/*
add_rank()
This function add 1 to the rank of this function.
*/

void Answer::add_rank(double new_rank) 
{
	rank += new_rank;
	return;
}