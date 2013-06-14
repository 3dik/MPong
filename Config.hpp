//Configuration class

#pragma once

#include <fstream>
#include <vector>
#include <locale>
#include <map>

#include "Convert.hpp"
#include "Err.hpp"
#include "Misc.hpp"
#include "Functions.hpp"


class Config
{
    public:

    Config ();
    ~Config ();
    //Initialize, the last argument states whether the entries are optional
    bool Init (Err *pErr, wstring sFileName, bool bOptional = false);
    //Returns the value of an entry being a string
    bool Get (wstring sName, wstring *psValue);
    //Returns the value of an entry being a number
    template <class tnNum>
    bool GetNum (wstring sName, tnNum *pnValue);
    //Returns the value of an entry being a boolean
    bool GetBool (wstring sName, bool *pbValue);
    //Sets the value of a entry being a string
    void Set (wstring sName, wstring sValue = L"");
    //Sets the value of an entry being a boolean
    void SetBool (wstring sName, bool bValue);
    //Saves all modified configuration entrys
    bool Save ();
    //Sets and loads a new configuration file
    bool SetFile (wstring sFileName);
    //Returns the configuration file name
    wstring GetFileName ();

    private:

    typedef std::map<wstring, wstring>::iterator EntryIterator;

    //Returns an iterator to the requested entry
    bool GetEntry (wstring sName, EntryIterator *piEntry);
    //Returns the next line of the configuration file
    bool GetLine (wstring *psLine, bool *pbLast, std::wfstream *pFile);
    //Loads the configuration file
    bool LoadConfig      ();

    Err                        m_Err;
    std::map<wstring, wstring> m_lsEntries;
    wstring                    m_sFileName;
    bool                       m_bOptional;

    const static int     s_nBufferLen;
    const static wstring s_sTrue;
    const static wstring s_sFalse;

};

#include "Config.inl"
