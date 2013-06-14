//Look at Config.hpp

template <class tnNum>
bool Config::GetNum (wstring sName, tnNum *pnValue)
{
    Err fErr (m_Err, L"GetNum");
    wstring sValue;
    if (!Get (sName, &sValue))
    {
        return false;
    }

    if (!StrToNum (sValue, pnValue))
    {
        return fErr.Set (L"Configuration entry is not numeric: " + sName);
    }

    return true;
}//GetNum
