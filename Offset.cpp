#include "stdafx.h"
#include "Offset.h"
using namespace std;

Offset::Offset(string new_key, int new_value)
{
	key = new_key;
	value = new_value;
}


Offset::~Offset()
{
}

string Offset::get_key()
{
	return key;
}

int Offset::get_value()
{
	return value;
}