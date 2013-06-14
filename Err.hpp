//Error class

#pragma once

#include <string>
#include <vector>

#include "Functions.hpp"

using std::wstring;

class Err
{
    public:

    Err ();
    ~Err ();
    //Sets the type to function error object and the function name
    Err (Err &ClassErr, wstring sName);
    //Sets the type to class error object and the class name
    bool SetClass (Err *pMainErr, wstring sName);
    //Get the address of the main error object
    Err *GetMain ();
    //Is this object a class error object? Otherwise: Its function error object
    bool IsClass ();
    //Is an error occurred?
    bool IsError ();
    //Sets an error
    bool Set (wstring sErr);
    //Adds a function to the list of the class error object
    void AddFunction (wstring sName);
    //Erases the last function from the list of the class error object
    void KickFunction ();

    private:

    std::vector<wstring> m_lsFunctions;
    wstring              m_sClassName;
    Err *                m_pMainErr; //Pointer to the main or class error object
    bool                 m_bError;
};//Err
