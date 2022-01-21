#include "stdafx.h"
#include <string>
#include "stemWords.h"
#include "stemming/english_stem.h"

std::string stemWords(std::string oldword)
{
    stemming::english_stem<> StemEnglish;
    
    int wordsize = oldword.length();

    //库函数以wstring格式字符串为输入，因此先把字符串转化为wstring格式
    wchar_t *transToWstring = new wchar_t[wordsize + 1];
    std::wmemset(transToWstring, 0, wordsize+1);
    std::mbstowcs(transToWstring, oldword.c_str(), wordsize);
    std::wstring transword = transToWstring;

    //调用库函数进行词干提取
    StemEnglish(transword);

    //将还原后的转换回string格式
    char *transToChar = new char[wordsize + 1];
    std::memset(transToChar, 0, wordsize + 1);
    std::wcstombs(transToChar, transword.c_str(), wordsize + 1);
    std::string newword(transToChar);

    return newword;
}