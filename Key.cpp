#include "Key.hpp"


std::map<wstring, Key::Input> Key::s_lInputs;
sf::Vector2<int>              Key::s_vOldMousePos;
int                           Key::s_nMouseSensitivity = 0;


//Key
Key::Key ()
{
    if (s_lInputs.empty ())
    {
        FillInputList ();
        s_vOldMousePos.x = 0;
        s_vOldMousePos.y = 0;
    }

    m_pWindow = NULL;
}//Key


//~Key
Key::~Key ()
{
    m_Input.nType = eNone;
}//~Key


//Init
bool Key::Init (Err *pErr, sf::RenderWindow *pWindow,
                wstring sName, bool bAutoMouseReset)
{
    if (!m_Err.SetClass (pErr, L"Key"))
    {
        return false;
    }
    Err fErr (m_Err, L"Init");
    if (pWindow == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    m_pWindow = pWindow;

    std::map<wstring, Input>::iterator iInput = s_lInputs.find (sName);
    if (iInput != s_lInputs.end ())
    {
        m_Input.nType = iInput->second.nType;
        m_Input.nCode = iInput->second.nCode;
    }
    m_bAutoMouseReset = bAutoMouseReset;

    ResetMouse ();

    return true;
}//Init


//IsPressed
bool Key::IsPressed (bool bMouse)
{
    if (!bMouse && m_Input.nType != eKeyboard)
    {
        return false;
    }

    switch (m_Input.nType)
    {
        case (eKeyboard):
            return m_pWindow->GetInput ().IsKeyDown (
                   static_cast<sf::Key::Code> (m_Input.nCode));
        case (eMouse):
            return m_pWindow->GetInput ().IsMouseButtonDown (
                   static_cast<sf::Mouse::Button> (m_Input.nCode));
        case (eMousePos):
            return CheckMouseMovement ();
        case (eNone):
            return false;
    }

    return false;
}//IsPressed


//SetMouseSensitivity
bool Key::SetMouseSensitivity (int nValue)
{
    if (nValue < 0)
    {
        return false;
    }

    s_nMouseSensitivity = nValue;

    return true;
}//SetMouseSensitivity


//GetInputName
wstring Key::GetInputName (const sf::RenderWindow &Window,
                           int nCode, enType nType)
{
    std::map<wstring, Input>::iterator i = s_lInputs.begin ();
    for (; i != s_lInputs.end (); ++i)
    {
        if (i->second.nType != nType || i->second.nCode != nCode)
        {
            continue;
        }
        return i->first;
    }

    return L"";
}//GetInputName


//CheckMouseMovement
bool Key::CheckMouseMovement ()
{
    bool bMoved = false;

    switch (m_Input.nCode)
    {
        case (eUp):
            if ((m_pWindow->GetInput ().GetMouseY () - s_vOldMousePos.y)
                < -s_nMouseSensitivity * m_pWindow->GetFrameTime ())
            {
                bMoved = true;
            }
            break;
        case (eDown):
            if ((m_pWindow->GetInput ().GetMouseY () - s_vOldMousePos.y)
                > s_nMouseSensitivity * m_pWindow->GetFrameTime ())
            {
                bMoved = true;
            }
            break;
        case (eLeft):
            if ((m_pWindow->GetInput ().GetMouseX () - s_vOldMousePos.x)
                < -s_nMouseSensitivity * m_pWindow->GetFrameTime ())
            {
                bMoved = true;
            }
            break;
        case (eRight):
            if ((m_pWindow->GetInput ().GetMouseX () - s_vOldMousePos.x)
                > s_nMouseSensitivity * m_pWindow->GetFrameTime ())
            {
                bMoved = true;
            }
            break;
    }

    if (m_bAutoMouseReset)
    {
        ResetMouse ();
    }

    return bMoved;
}//CheckMouseMovement


//FillInputList
void Key::FillInputList ()
{
    using namespace sf::Key;

    AddInput (L"A", A);
    AddInput (L"B", B);
    AddInput (L"C", C);
    AddInput (L"D", D);
    AddInput (L"E", E);
    AddInput (L"F", F);
    AddInput (L"G", G);
    AddInput (L"H", H);
    AddInput (L"I", I);
    AddInput (L"J", J);
    AddInput (L"K", K);
    AddInput (L"L", L);
    AddInput (L"M", M);
    AddInput (L"N", N);
    AddInput (L"O", O);
    AddInput (L"P", P);
    AddInput (L"Q", Q);
    AddInput (L"R", R);
    AddInput (L"S", S);
    AddInput (L"T", T);
    AddInput (L"U", U);
    AddInput (L"V", V);
    AddInput (L"W", W);
    AddInput (L"X", X);
    AddInput (L"Y", Y);
    AddInput (L"Z", Z);
    AddInput (L"0", Num0);
    AddInput (L"1", Num1);
    AddInput (L"2", Num2);
    AddInput (L"3", Num3);
    AddInput (L"4", Num4);
    AddInput (L"5", Num5);
    AddInput (L"6", Num6);
    AddInput (L"7", Num7);
    AddInput (L"8", Num8);
    AddInput (L"9", Num9);
    AddInput (L"Pad0", Numpad0);
    AddInput (L"Pad1", Numpad1);
    AddInput (L"Pad2", Numpad2);
    AddInput (L"Pad3", Numpad3);
    AddInput (L"Pad4", Numpad4);
    AddInput (L"Pad5", Numpad5);
    AddInput (L"Pad6", Numpad6);
    AddInput (L"Pad7", Numpad7);
    AddInput (L"Pad8", Numpad8);
    AddInput (L"Pad9", Numpad9);
    AddInput (L"F1", F1);
    AddInput (L"F2", F2);
    AddInput (L"F3", F3);
    AddInput (L"F4", F4);
    AddInput (L"F5", F5);
    AddInput (L"F6", F6);
    AddInput (L"F7", F7);
    AddInput (L"F8", F8);
    AddInput (L"F9", F9);
    AddInput (L"F10", F10);
    AddInput (L"F11", F11);
    AddInput (L"F12", F12);
    AddInput (L"F13", F13);
    AddInput (L"F14", F14);
    AddInput (L"F15", F15);
    AddInput (L"Left", Left);
    AddInput (L"Right", Right);
    AddInput (L"Up", Up);
    AddInput (L"Down", Down);
    AddInput (L"Return", Return);
    AddInput (L"Space", Space);
    AddInput (L"Control Left", LControl);
    AddInput (L"Control Right", RControl);
    AddInput (L"Shift Left", LShift);
    AddInput (L"Shift Right", RShift);
    AddInput (L"Alt Left", LAlt);
    AddInput (L"Alt Right", RAlt);
    AddInput (L"+", Add);
    AddInput (L"-", Subtract);
    AddInput (L"*", Multiply);
    AddInput (L"/", Divide);
    AddInput (L"Backspace", Back);
    AddInput (L"Tab", Tab);
    AddInput (L"PageUp", PageUp);
    AddInput (L"PageDown", PageDown);
    AddInput (L"End", End);
    AddInput (L"Home", Home);
    AddInput (L"Insert", Insert);
    AddInput (L"Delete", Delete);

    AddInput (L"MouseClickLeft", sf::Mouse::Left, eMouse);
    AddInput (L"MouseClickRight", sf::Mouse::Right, eMouse);
    AddInput (L"MouseClickMiddle", sf::Mouse::Middle, eMouse);

    AddInput (sMOUSE_UP, eUp, eMousePos);
    AddInput (sMOUSE_DOWN, eDown, eMousePos);
    AddInput (sMOUSE_LEFT, eLeft, eMousePos);
    AddInput (sMOUSE_RIGHT, eRight, eMousePos);
}//FillInputList


//AddInput
void Key::AddInput (wstring sName, unsigned int nCode, enType nType)
{
    Input fInput;
    fInput.nType = nType;
    fInput.nCode = nCode;
    s_lInputs.insert (std::make_pair (sName, fInput));
}//AddInput


//ResetMouse
void Key::ResetMouse ()
{
    if (m_Input.nType != eMousePos)
    {
        return;
    }
    s_vOldMousePos.x = m_pWindow->GetInput ().GetMouseX ();
    s_vOldMousePos.y = m_pWindow->GetInput ().GetMouseY ();
}//ResetMouse
