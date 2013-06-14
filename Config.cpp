#include "Config.hpp"

const int Config::s_nBufferLen = 10;
const wstring Config::s_sTrue = L"true";
const wstring Config::s_sFalse = L"false";


//Config
Config::Config ()
{
}//Config


//~Config
Config::~Config ()
{
}//~Config


//Init
bool Config::Init (Err *pErr, wstring sFileName, bool bOptional)
{
    if (!m_Err.SetClass (pErr, L"Config"))
    {
        return false;
    }
    m_bOptional = bOptional;
    return SetFile (sFileName);
}//Init


//Get
bool Config::Get (wstring sName, wstring *psValue)
{
    Err fErr (m_Err, L"Get");
    if (psValue == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    if (m_bOptional)
    {
        *psValue = m_lsEntries[sName];
        return true;
    }

    EntryIterator iEntry;
    if (!GetEntry (sName, &iEntry))
    {
        return false;
    }
    *psValue = iEntry->second;

    return true;
}//Get


//GetBool
bool Config::GetBool (wstring sName, bool *pbValue)
{
    Err fErr (m_Err, L"GetBool");
    wstring sValue;
    if (!Get (sName, &sValue))
    {
        return false;
    }

    if (sValue == s_sTrue)
    {
        *pbValue = true;
    }
    else if (sValue == s_sFalse)
    {
        *pbValue = false;
    }
    else
    {
        return fErr.Set (L"Configuration entry is not a boolean: " + sName);
    }

    return true;
}//GetBool


//Set
void Config::Set (wstring sName, wstring sValue)
{
    m_lsEntries[sName] = sValue;
}//Set


//SetBool
void Config::SetBool (wstring sName, bool bValue)
{
    wstring sValue = s_sFalse;
    if (bValue)
    {
        sValue = s_sTrue;
    }

    Set (sName, sValue);
}//SetBool


//Save
bool Config::Save ()
{
    Err fErr (m_Err, L"Save");

    std::wofstream File;
    File.open (std::string (m_sFileName.begin (), m_sFileName.end ()).c_str ());
    if (!File.good ())
    {
        fErr.Set (sERR_FILEFAIL + m_sFileName);
        return false;
    }

    for (EntryIterator i = m_lsEntries.begin (); i != m_lsEntries.end (); ++i)
    {
        wstring sLine = i->first;
        if (!i->second.empty ())
        {
            sLine += L" " + i->second;
        }
        File<<sLine<<std::endl;
    }
    File.close ();

    return true;
}//Save


//SetFile
bool Config::SetFile (wstring sFileName)
{
    Err fErr (m_Err, L"SetFile");
    if (sFileName.empty ())
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    m_sFileName = sFileName;

    return LoadConfig ();
}//SetFile


//GetFileName
wstring Config::GetFileName ()
{
    size_t nFileNamePos = m_sFileName.rfind (L"/");
    if (nFileNamePos == wstring::npos)
    {
        return m_sFileName;
    }
    return m_sFileName.substr (nFileNamePos + 1);
}//GetFileName


//GetEntry
bool Config::GetEntry (wstring sName, EntryIterator *piEntry)
{
    Err fErr (m_Err, L"GetEntry");
    if (sName.empty () || piEntry == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    *piEntry = m_lsEntries.find (sName);
    if (*piEntry == m_lsEntries.end ())
    {
        return fErr.Set (L"Requested configuration entry not found: " + sName);
    }

    return true;
}//GetEntry


//GetLine
bool Config::GetLine (wstring *psLine, bool *pbLast, std::wfstream *pFile)
{
    Err fErr (m_Err, L"GetLine");
    if (psLine == NULL || pbLast == NULL || pFile == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    psLine->clear ();
    *pbLast = false;

    while (true)
    {
        wchar_t psBuffer[s_nBufferLen];
        pFile->read (psBuffer, s_nBufferLen);
        if (pFile->bad ())
        {
            return fErr.Set (L"Could not read file");
        }
        psLine->append (psBuffer, pFile->gcount ());

        int nLineLen = psLine->find (sENDL);
        if (nLineLen != -1)
        {
            pFile->seekg (static_cast<int> (pFile->tellg ()) - (psLine->size () - nLineLen) + 1);
            *psLine = psLine->substr (0, nLineLen);
            break;
        }

        if (pFile->eof ())
        {
            *pbLast = true;
            break;
        }
    }

    return true;
}//GetLine


//LoadConfig
bool Config::LoadConfig ()
{
    Err fErr (m_Err, L"LoadConfig");

    m_lsEntries.clear ();
    std::wfstream File;
    File.open (ConStr<char> (m_sFileName).c_str (),
               std::ios_base::out | std::ios_base::in | std::ios_base::app);
    if (!File.good ())
    {
        fErr.Set (sERR_FILEFAIL + m_sFileName);
        return false;
    }

    while (true)
    {
        wstring sLine;
        bool bLast = false;
        if (GetLine (&sLine, &bLast, &File) == false)
        {
            return false;
        }
        if (bLast)
        {
            break;
        }

        if (sLine.empty ())
        {
            continue;
        }

        wstring sName, sValue;
        size_t nNameLen = sLine.find (L" ");
        if (nNameLen == wstring::npos)
        {
            sName = sLine;
        }
        else
        {
            if (nNameLen == 0)
            {
                return fErr.Set (L"Entries must begin on the first character");
            }
            sName = sLine.substr (0, nNameLen);
            sLine = sLine.substr (nNameLen);
            size_t nVarPos = sLine.find_first_not_of (L" ");
            if (nVarPos != wstring::npos)
            {
                sValue = sLine.substr (nVarPos);
            }
        }

        m_lsEntries.insert (std::make_pair (sName, sValue));
    };
    File.close ();

    return true;
}//LoadConfig
