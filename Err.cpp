#include "Err.hpp"

//Err
Err::Err ()
{
    m_bError = false;
    m_pMainErr = NULL;
}//Err


//~Err
Err::~Err ()
{
    if (!IsClass () && m_pMainErr != NULL)
    {
        m_pMainErr->KickFunction ();
    }
}//~Err


//Err
Err::Err (Err &MainErr, wstring sName)
{
    Err ();
    if (!MainErr.IsClass ())
    {
        return;
    }
    m_pMainErr =  &MainErr;
    m_pMainErr->AddFunction (sName);
}//Err


//SetClass
bool Err::SetClass (Err *pMainErr, wstring sName)
{
    if (pMainErr == NULL || sName.empty ()) return false;

    m_pMainErr = pMainErr->GetMain ();
    m_sClassName = sName;
    m_lsFunctions.push_back (L"Unspecified");
    return true;
}//SetClass


//GetMain
Err *Err::GetMain ()
{
    if (m_pMainErr == NULL)
    {
        return this;
    }
    return m_pMainErr->GetMain ();;
}//GetMain


//IsClass
bool Err::IsClass ()
{
    if (m_lsFunctions.size () == 0)
    {
        return false;
    }
    return true;
}//IsClass


//IsError
bool Err::IsError ()
{
    if (m_pMainErr != NULL)
    {
        return m_pMainErr->GetMain ()->IsError ();
    }

    if (m_bError)
    {
        return true;
    }
    return false;
}//IsError


//Set
bool Err::Set (wstring sErr)
{
    if (sErr.empty ())
    {
        sErr = L"Error text not specified";
    }

    if (m_pMainErr == NULL) //This object is the main error object
    {
        OPut (L"Error: " + sErr);
        m_bError = true;
        return false;
    }
    if (!IsClass ())
    {
        m_pMainErr->Set (sErr); //The class error object process further
        return false;
    }

    m_pMainErr->GetMain ()->Set (m_sClassName +
                                 L"->" +
                                 m_lsFunctions.back () +
                                 L" : " +
                                 sErr);
    return false;
}//Set


//AddFunction
void Err::AddFunction (wstring sName)
{
    if (!IsClass ())
    {
        return;
    }
    if (sName.empty ())
    {
        sName = L"Unspecified";
    }

    m_lsFunctions.push_back (sName);
}//AddFunction


//KickFunction
void Err::KickFunction ()
{
    if (m_lsFunctions.size () == 0)
    {
        return;
    }
    m_lsFunctions.pop_back ();
}//KickFunction
