//Some conversion functions

#pragma once

#include <sstream>
#include <string>

using std::wstring;

//String to number
template <class tnNum>
bool StrToNum (wstring sStr, tnNum *pnVal);

//Checks whether the given string is numeric
bool IsNumeric (wstring sStr);

//Number to string
template <class tnNum>
wstring NumToStr (tnNum nVal);

//Converts between String or WString
template <class tsTo, class tsFrom>
std::basic_string<tsTo> ConStr (std::basic_string<tsFrom> sVal);

#include "Convert.inl"
