//Manages a single "user action". An user action is an abstract user input like
//"shoot" or "go".

#pragma once

#include <map>
#include "SFML/Graphics.hpp"

#include "Err.hpp"

class Key
{
    public:

    enum enType
    {
        eNone,
        eKeyboard,
        eMouse,
        eMousePos
    };

    Key ();
    ~Key ();
    //Initializes and interpret the given input code
    bool Init (Err *pErr, sf::RenderWindow *pWindow,
               wstring sName, bool bAutoMouseReset = false);
    //Is the saved key pressed?
    bool IsPressed (bool bMouseMovement = true);
    //Resets the "old" mouse position to the current
    void ResetMouse ();

    //Sets the mouse sensitivity
    static bool SetMouseSensitivity (int nValue);
    //Returns the readable name of the given input code
    static wstring GetInputName (const sf::RenderWindow &Window,
                                 int nCode, enType nType = eKeyboard);

    private:

    //Checks whether the mouse was moved
    bool CheckMouseMovement ();

    enum enMouseDirection
    {
        eUp,
        eDown,
        eLeft,
        eRight
    };
    struct Input
    {
        enType nType;
        int nCode;
    };

    //Fills out the input list
    static void FillInputList ();
    //Adds a signel input object to the list
    static void AddInput (wstring sName, unsigned int nCode,
                          enType nType=eKeyboard);

    Input             m_Input;
    Err               m_Err;
    sf::RenderWindow *m_pWindow;
    bool              m_bAutoMouseReset;

    static std::map<wstring, Input> s_lInputs;
    static sf::Vector2<int>         s_vOldMousePos;
    static int                      s_nMouseSensitivity;
};
