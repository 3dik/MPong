#include "Convert.hpp"

//IsNumeric
bool IsNumeric (wstring sStr)
{
    int nVal;
    return StrToNum (sStr, &nVal);
}//IsNumeric
