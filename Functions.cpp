#include "Functions.hpp"


//ListFiles
bool ListFiles (wstring sPath, std::vector<wstring> *plsFiles)
{
    if (sPath.empty () || plsFiles == NULL)
    {
        return false;
    }

    DIR *pFile = opendir (ConStr<char> (sPath).c_str ());
    if (pFile == NULL)
    {
        return false;
    }
    dirent *pEntry;
    while (true)
    {
        pEntry = readdir (pFile);
        if (pEntry == NULL)
        {
            break;
        }
        wstring sFile = ConStr<wchar_t> (std::string (pEntry->d_name));
        if (sFile == L"." || sFile == L"..")
        {
            continue;
        }

        struct stat FileInfo;
        int nFileInfoLen = stat (ConStr<char> (sPath + sFile).c_str (), &FileInfo);
        if (nFileInfoLen == -1)
        {
            closedir (pFile);
            pFile = NULL;
            break;
        }

        if (S_ISREG (FileInfo.st_mode))
        {
            if (!FilterExtension (sFile))
            {
                continue;
            }
            plsFiles->push_back (sFile);
        }
    }

    if (pFile == NULL)
    {
        return false;
    }
    if (closedir (pFile) == -1)
    {
        return false;
    }
    return true;
}//ListFiles


//FilterExtension
bool FilterExtension (wstring sFileName, wstring *psValue)
{
    if (sFileName.empty () || sFileName.size () <= sFILEEXTENSION.size ())
    {
        return false;
    }

    int nExtensionPos = sFileName.size () - sFILEEXTENSION.size ();
    if (sFileName.substr (nExtensionPos) != sFILEEXTENSION)
    {
        return false;
    }

    if (psValue != NULL)
    {
        *psValue = sFileName.substr (0, nExtensionPos);
    }

    return true;
}//FilterExtension


//CalcFuncFactors
bool CalcFuncFactors (Vector2f vBegin, Vector2f vEnd,
                      float *pnM, float *pnB, bool *pbForward)
{
    if (pnM == NULL || pnB == NULL)
    {
        return false;
    }

    if (pbForward != NULL)
    {
        *pbForward = true;
    }
    //Classic mathematic function, from left to right
    Vector2f *pvLeft = &vBegin, *pvRight = &vEnd;
    if (vBegin.x > vEnd.x)
    {
        pvLeft = &vEnd;
        pvRight = &vBegin;
        if (pbForward != NULL)
        {
            *pbForward = false;
        }
    }
    Vector2f vRel (*pvRight - *pvLeft);
    *pnM = vRel.y / vRel.x;
    *pnB = vBegin.y - (*pnM * vBegin.x);

    return true;
}//CalcFuncFactors






