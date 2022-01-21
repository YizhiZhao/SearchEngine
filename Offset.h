#pragma once
#include <string>

class Offset
{
public:
	Offset(std::string, int);
	~Offset();
	std::string get_key();
	int get_value();

private:
	std::string key;
	int value;
};

