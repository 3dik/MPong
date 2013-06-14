//Look at Convert.hpp

//StrToNum
template <class tnNum>
bool StrToNum (wstring sStr, tnNum *pnVal)
{
    if (sStr.empty () || pnVal == NULL)
    {
        return false;
    }

    std::wistringstream sNum (sStr);
    if (!(sNum >> *pnVal))
    {
        return false;
    }

    return true;
}//StrToNum


//NumToStr
template <class tnNum>
wstring NumToStr (tnNum nVal)
{
    std::wstringstream sNum;
    sNum<<nVal;
    return sNum.str ();
}//NumToStr


//ConStr
template <class tsTo, class tsFrom>
std::basic_string<tsTo> ConStr (std::basic_string<tsFrom> sVal)
{
    return std::basic_string<tsTo> (sVal.begin (), sVal.end ());
}//ConStr
