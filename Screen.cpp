#include "Screen.hpp"

const Vector2f  Screen::s_vButtonSize (200, 50);
//const int       Screen::s_nTransparency = 32;
const int       Screen::s_nEditValueMargin = 5;
const int       Screen::s_nListValueMargin = 10;
const wstring   Screen::s_sErrWidgetType = L"The given widget has a wrong type";
const wstring   Screen::s_sErrListValue  = L"The given entry id is not numeric";
const sf::Color Screen::s_UnselectColor (255, 255, 255, 64);

bool Screen::s_bPressed = false;

struct Screen::Widget
{
    Vector2f             vPos;
    Vector2f             vSize;
    wstring              sText;
    enType               nType;
    unsigned int         nId;
    wstring              sValue;
    wstring              sStartValue;
    std::vector<wstring> lsEntries;
};


//Screen
Screen::Screen ()
{
    m_pProfiles = NULL;
    m_pWindow = NULL;

    m_nState = eStop;
    m_nSelectedWidget = -1;
}//Screen


//~Screen
Screen::~Screen ()
{
}//~Screen


//Init
bool Screen::Init (Err *pErr, Profiles *pProfiles,
                   sf::RenderWindow *pWindow, wstring sCaption)
{
    Screen ();
    if (!m_Err.SetClass (pErr, L"Screen") ||
        !Render::Init (pErr, pWindow))
    {
        return false;
    }
    Err fErr (m_Err, L"Init");
    if (pProfiles == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    m_lWidgets.clear ();

    m_pProfiles = pProfiles;
    m_sCaption = sCaption;

    return Reset ();
}//Init


//AddLabel
bool Screen::AddLabel (wstring sCaption, Vector2f vPos)
{
    Widget Label;
    if (!FillWidget (&Label, eLabel, sCaption, vPos))
    {
        return false;
    }
    m_lWidgets.push_back (Label);

    return true;
}//AddLabel


//AddButton
bool Screen::AddButton (wstring  sCaption, Vector2f vPos, unsigned int nId)
{
    Widget Button;
    if (!FillWidget (&Button, eButton, sCaption, vPos))
    {
        return false;
    }
    Button.nId = nId;
    Button.vSize = Vector2f (200, 50);
    m_lWidgets.push_back (Button);

    return true;
}//AddButton


//AddCheckbox
bool Screen::AddCheckbox (wstring sCaption, Vector2f vPos,
                          unsigned int nId, bool bValue)
{
    Widget Checkbox;
    if (!FillWidget (&Checkbox, eCheckbox, sCaption, vPos))
    {
        return false;
    }
    Checkbox.nId = nId;
    if (bValue)
    {
        Checkbox.sStartValue = L"1";
    }

    sf::String Caption (Checkbox.sText, sf::Font::GetDefaultFont ());
    Checkbox.vSize.x = Caption.GetRect ().GetWidth () +
                       Caption.GetRect ().GetHeight () + 30;
    Checkbox.vSize.y = Caption.GetRect ().GetHeight ();

    m_lWidgets.push_back (Checkbox);

    return true;
}//AddCheckbox


//AddEditbox
bool Screen::AddEditbox (wstring sCaption, Vector2f vPos, unsigned int nSize,
                         unsigned int nId, wstring sValue)
{
    Widget Editbox;
    if (!FillWidget (&Editbox, eEditbox, sCaption, vPos))
    {
        return false;
    }
    Editbox.nId = nId;
    Editbox.sStartValue = sValue;

    sf::String Caption (Editbox.sText + L"|", sf::Font::GetDefaultFont ());
    Editbox.vSize.x = Caption.GetRect ().GetWidth () + nSize;
    Editbox.vSize.y = Caption.GetRect ().GetHeight ();

    m_lWidgets.push_back (Editbox);

    return true;
}//AddEditbox


//AddList
bool Screen::AddList (wstring sCaption, Vector2f vPos,
                      const std::vector<wstring> &lsEntries,
                      unsigned int nValue, unsigned int nId)
{
    Err fErr (m_Err, L"AddList");
    if (nValue == lsEntries.size ())
    {
        return fErr.Set (L"The given list entry does not exists");
    }
    Widget List;
    if (!FillWidget (&List, eList, sCaption, vPos))
    {
        return false;
    }
    List.nId = nId;
    List.lsEntries = lsEntries;
    List.sStartValue = NumToStr (nValue);

    size_t nLen = 0;
    for (unsigned int i = 0; i != List.lsEntries.size (); i++)
    {
        sf::String Entry (List.lsEntries.at (i), sf::Font::GetDefaultFont ());
        if (Entry.GetRect ().GetWidth () > nLen)
        {
            nLen = Entry.GetRect ().GetWidth ();
        }
    }

    sf::String Caption (List.sText + L":", sf::Font::GetDefaultFont ());

    List.vSize.x = Caption.GetRect ().GetWidth () + s_nListValueMargin + nLen;
    List.vSize.y = Caption.GetRect ().GetHeight ();

    m_lWidgets.push_back (List);

    return true;
}//AddList


//Run
bool Screen::Run ()
{
    Err fErr (m_Err, L"Run");
    if (m_nState != eRun)
    {
        return fErr.Set (L"Screen has a wrong state");
    }

    for (unsigned int i = 0; i != m_lWidgets.size (); i++)
    {
        m_lWidgets.at (i).sValue = m_lWidgets.at (i).sStartValue;
    }
    m_bWidgetActive = false;

    while (m_nState == eRun)
    {
        sf::Event Event;
        while (m_pWindow->GetEvent (Event))
        {
            switch (Event.Type)
            {
                case (sf::Event::Closed):
                    m_nState = eQuit;
                    break;
                case (sf::Event::KeyPressed):
                    if (Event.Key.Code == sf::Key::Escape)
                    {
                        m_nState = eQuit;
                    }
                    break;
                case (sf::Event::MouseWheelMoved):
                    if (s_bPressed)
                    {
                        break;
                    }
                    if (Event.MouseWheel.Delta > 0)
                    {
                        if (!Direction (false))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (!Direction (true))
                        {
                            return false;
                        }
                    }
                    break;
                case (sf::Event::TextEntered):
                    if (m_bWidgetActive && GetCurWidget ().nType == eEditbox)
                    {
                        if (!AddEditValue (Event.Text.Unicode))
                        {
                            return false;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        if (!ManageInputs () ||
            !Render ())
        {
            return false;
        }

        sf::Sleep (nSLEEPTIME);
    }

    return true;
}//Run


//GetChecked
bool Screen::GetChecked (unsigned int nId, bool *pbValue)
{
    Err fErr (m_Err, L"GetChecked");
    if (pbValue == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    Widget *pWidget;
    if (!GetWidgetById (nId, &pWidget))
    {
        return false;
    }

    *pbValue = false;
    if (!pWidget->sValue.empty ())
    {
        *pbValue = true;
    }

    return true;
}//GetChecked


//Get
bool Screen::Get (unsigned int nId, wstring *psValue)
{
    Err fErr (m_Err, L"Get");
    if (psValue == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    Widget *pWidget;
    if (!GetWidgetById (nId, &pWidget))
    {
        return false;
    }

    if (pWidget->nType == eEditbox)
    {
        *psValue = pWidget->sValue;
        return true;
    }

    int nEntryId;
    if (!StrToNum (pWidget->sValue, &nEntryId))
    {
        return fErr.Set (s_sErrListValue);
    }
    *psValue = pWidget->lsEntries.at (nEntryId);

    return true;
}//Get


//IsQuit
bool Screen::IsQuit ()
{
    if (m_nState == eQuit)
    {
        return true;
    }
    return false;
}//IsQuit


//Reset
bool Screen::Reset ()
{
    m_nState = eRun;
    m_nSelectedWidget = -1;

    return true;
}//Reset


//GetSelectId
bool Screen::GetSelectId (unsigned int *pnId)
{
    Err fErr (m_Err, L"GetSelectId");
    if (pnId == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    if (!CheckSelectId ())
    {
        return false;
    }
    if (GetCurWidget ().nType != eButton)
    {
        return fErr.Set (L"The selected widget must be a button");
    }

    *pnId = GetCurWidget ().nId;

    return true;
}//GetSelectId


//AddEditValue
bool Screen::AddEditValue (sf::Uint32 nChar)
{
    if (nChar == 13) //return key
    {
        m_bWidgetActive = false;
        return true;
    }
    if (nChar == 8) //backspace
    {
        size_t nValueLen = GetCurWidget ().sValue.size ();
        if (nValueLen == 0)
        {
            return true;
        }
        m_lWidgets.at (m_nSelectedWidget).sValue.resize (nValueLen - 1);
        return true;
    }

    wchar_t cChar = static_cast<wchar_t> (nChar);
    sf::String CheckString (GetCurWidget ().sText + GetCurWidget ().sValue
                            + L"|" + cChar,
                            sf::Font::GetDefaultFont ());
    if (CheckString.GetRect ().GetWidth () < GetCurWidget ().vSize.x -
                                             s_nEditValueMargin)
    {
        m_lWidgets.at (m_nSelectedWidget).sValue += cChar;
    }

    return true;
}//AddEditValue


//GetWidgetById
bool Screen::GetWidgetById (unsigned int nId, Widget **ppWidget)
{
    Err fErr (m_Err, L"GetWidgetById");
    if (ppWidget == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    for (unsigned int i = 0; i != m_lWidgets.size (); i++)
    {
        if (m_lWidgets.at (i).nId == nId)
        {
            *ppWidget = &m_lWidgets.at (i);
            return true;
        }
    }

    return fErr.Set (L"A widget with the given id does not exists");
}//GetWidgetById


//IncreaseSelectId
bool Screen::IncreaseSelectId (bool bNext)
{
    Err fErr (m_Err, L"IncreaseSelectId");
    if (m_lWidgets.size () == 0)
    {
        return fErr.Set (L"They are not widgets in the current menu screen");
    }

    int nLeftWidgets = m_lWidgets.size ();
    do
    {
        if (bNext)
        {
            m_nSelectedWidget ++;
            if (m_nSelectedWidget == static_cast<int> (m_lWidgets.size ()))
            {
                m_nSelectedWidget = 0;
            }
        }
        else
        {
            m_nSelectedWidget --;
            if (m_nSelectedWidget < 0)
            {
                m_nSelectedWidget = m_lWidgets.size () - 1;
            }
        }

        nLeftWidgets --;
        if (nLeftWidgets < 0)
        {
            m_nSelectedWidget = -1;
            return true;
        }
    } while (GetCurWidget ().nId == 0);

    return true;
}//IncreaseSelectId


//MouseSelect
bool Screen::MouseSelect ()
{
    Err fErr (m_Err, L"MouseSelect");

    Vector2f vMousePos (m_pWindow->GetInput ().GetMouseX (),
                        m_pWindow->GetInput ().GetMouseY ());
    vMousePos = Rel (vMousePos);
    if (vMousePos == m_vMousePos)
    {
        return true;
    }
    m_vMousePos = vMousePos;

    if (m_bWidgetActive)
    {
        return true;
    }

    for (unsigned int i = 0; i != m_lWidgets.size (); i++)
    {
        if (m_lWidgets.at (i).nId == 0) //Only selectable widgets has an id
        {
            continue;
        }

        if (IsMouseOverWidget (m_lWidgets.at (i)))
        {
            m_nSelectedWidget = i;
            return true;
        }
    }

    return true;
}//MouseSelect


//ManageInputs
bool Screen::ManageInputs ()
{
    Err fErr (m_Err, L"ManageInputs");

    bool bSpecial, bLeft, bRight, bMouseLeft, bMouseMiddle, bMouseRight;
    bMouseLeft = m_pWindow->GetInput ().IsMouseButtonDown (sf::Mouse::Left);
    bMouseMiddle = m_pWindow->GetInput ().IsMouseButtonDown (sf::Mouse::Middle);
    bMouseRight = m_pWindow->GetInput ().IsMouseButtonDown (sf::Mouse::Right);

    if (!m_pProfiles->GetInput (eSpecial, &bSpecial, false) ||
        !m_pProfiles->GetInput (eLeft, &bLeft, false) ||
        !m_pProfiles->GetInput (eRight, &bRight, false))
    {
        return false;
    }

    if (m_bWidgetActive && GetCurWidget ().nType == eEditbox)
    {
        return true;
    }

    bool bCurPressed = false;
    if (bSpecial || bLeft || bRight ||
        bMouseLeft || bMouseMiddle || bMouseRight)
    {
        bCurPressed = true;
    }
    if (s_bPressed)
    {
        if (bCurPressed)
        {
            return true;
        }
        s_bPressed = false;
    }
    else if (!MouseSelect ())
    {
        return false;
    }
    if (bCurPressed)
    {
        s_bPressed = true;
    }

    if (bLeft)
    {
        return Direction (false);
    }
    if (bRight)
    {
        return Direction (true);
    }

    if (m_nSelectedWidget == -1)
    {
        return true;
    }

    if (bMouseLeft)
    {
        if (m_bWidgetActive)
        {
            m_bWidgetActive = false;
            return true;
        }
        if (IsMouseOverWidget (GetCurWidget ()))
        {
            return ExecuteWidgetAction ();
        }
        m_nSelectedWidget = -1;
        return true;
    }

    if (bMouseMiddle || bSpecial)
    {
        return ExecuteWidgetAction ();
    }

    if (bMouseRight)
    {
        m_bWidgetActive = false;
        m_nSelectedWidget = -1;
    }

    return true;
}//ManageInputs


//ExecuteWidgetAction
bool Screen::ExecuteWidgetAction ()
{
    Err fErr (m_Err, L"ExecuteWidgetAction");

    switch (GetCurWidget ().nType)
    {
        case (eButton):
            m_nState = eStop;
            break;
        case (eCheckbox):
            {
                if (GetCurWidget ().sValue.empty ())
                {
                    m_lWidgets.at (m_nSelectedWidget).sValue = L"1";
                    break;
                }
                m_lWidgets.at (m_nSelectedWidget).sValue.clear ();
                break;
            }
        case (eEditbox):
        case (eList):
            if (m_bWidgetActive)
            {
                m_bWidgetActive = false;
                break;
            }
            m_bWidgetActive = true;
            break;
        default:
            return fErr.Set (s_sErrWidgetType);
    }

    return true;
}//ExecuteWidgetAction


//Direction
bool Screen::Direction (bool bNext)
{
    if (m_bWidgetActive)
    {
        if (GetCurWidget ().nType == eList)
        {
            return IncreaseListEntryId (bNext);
        }
        return true;
    }

    return IncreaseSelectId (bNext);
}//Direction


//IncreaseListEntryId
bool Screen::IncreaseListEntryId (bool bNext)
{
    Err fErr (m_Err, L"IncreaseEntryId");

    int nCurEntryId;
    if (!StrToNum (GetCurWidget ().sValue, &nCurEntryId))
    {
        return fErr.Set (s_sErrListValue);
    }

    if (bNext)
    {
        nCurEntryId ++;
        if (nCurEntryId == static_cast<int> (GetCurWidget ().lsEntries.size ()))
        {
            nCurEntryId = 0;
        }
    }
    else
    {
        nCurEntryId --;
        if (nCurEntryId < 0)
        {
            nCurEntryId = GetCurWidget ().lsEntries.size () - 1;
        }
    }
    m_lWidgets.at (m_nSelectedWidget).sValue = NumToStr (nCurEntryId);

    return true;
}//IncreaseListEntryId


//Render
bool Screen::Render ()
{
    m_pWindow->Clear ();

    if (!RenderHeader ())
    {
        return false;
    }

    for (unsigned int i = 0; i != m_lWidgets.size (); i++)
    {
        switch (m_lWidgets.at (i).nType)
        {
            case (eLabel):
                if (!RenderLabel (m_lWidgets.at (i)))
                {
                    return false;
                }
                break;
            case (eButton):
                if (!RenderButton (m_lWidgets.at (i)))
                {
                    return false;
                }
                break;
            case (eCheckbox):
                if (!RenderCheckbox (m_lWidgets.at (i)))
                {
                    return false;
                }
                break;
            case (eEditbox):
                if (!RenderEditbox (m_lWidgets.at (i)))
                {
                    return false;
                }
                break;
            case (eList):
                if (!RenderList (m_lWidgets.at (i)))
                {
                    return false;
                }
                break;
        }
    }

    m_pWindow->Display ();

    return true;
}//Render


//RenderHeader
bool Screen::RenderHeader ()
{
    sf::String Name (sTITLE_SHORT, sf::Font::GetDefaultFont ());
    Name.SetSize (70);
    Name.SetPosition (10, 0);
    Draw (Name);

    Name.SetText (sTITLE_END);
    Name.Move (205, 45);
    Name.SetCenter (Vector2f (Name.GetRect ().GetWidth () / 2,
                              Name.GetRect ().GetHeight () / 2));
    float nAngle = 0;
    if (m_nSelectedWidget != -1)
    {
        Vector2f vAbsPos, vRelPos;
        if (!GetWidgetCenter (GetCurWidget (), &vAbsPos))
        {
            return false;
        }
        vRelPos.x = vAbsPos.x - Name.GetPosition ().x;
        vRelPos.y = vAbsPos.y - Name.GetPosition ().y;
        nAngle = atan (vRelPos.x / vRelPos.y) * 360.0f / (2.0f * nPI);
        if (vRelPos.y < 0)
        {
            nAngle -= 180;
        }
    }
    Name.SetRotation (nAngle);
    Draw (Name);

    sf::String Caption (m_sCaption, sf::Font::GetDefaultFont ());
    Caption.SetPosition (500, 35);
    Caption.SetCenter (Caption.GetRect ().GetWidth () / 2, 0);
    Draw (Caption);

    sf::Shape Line = sf::Shape::Rectangle (0, 70, 1000, 72, sf::Color::White);
    Draw (Line);

    return true;
}//RenderHeader


//RenderLabel
bool Screen::RenderLabel (const Widget &Label)
{
    sf::String Caption (Label.sText, sf::Font::GetDefaultFont ());
    Caption.SetPosition (Label.vPos);
    Draw (Caption);

    return true;
}//RenderLabel


//RenderButton
bool Screen::RenderButton (const Widget &Button)
{
    sf::String Caption (Button.sText, sf::Font::GetDefaultFont ());
    Vector2f vCenter;
    if (!GetWidgetCenter (Button, &vCenter))
    {
        return false;
    }
    Caption.SetPosition (vCenter);
    Caption.SetCenter (Caption.GetRect ().GetWidth () / 2,
                       Caption.GetRect ().GetHeight () / 2);
    Draw (Caption);

    sf::Color BorderColor = sf::Color::White;
    if (!IsWidgetSelected (Button))
    {
        BorderColor = s_UnselectColor;
    }

    RenderRect (Button.vPos, s_vButtonSize, sf::Color::Black, BorderColor);

    return true;
}//RenderButton


//RenderCheckbox
bool Screen::RenderCheckbox (const Widget &Checkbox)
{
    sf::String Caption (Checkbox.sText + L":", sf::Font::GetDefaultFont ());
    Caption.SetPosition (Checkbox.vPos);
    Draw (Caption);

    sf::Color InsideColor = sf::Color::Black;
    if (!Checkbox.sValue.empty ())
    {
        InsideColor = sf::Color::White;
    }
    Vector2f vBoxPos (Checkbox.vPos.x + Checkbox.vSize.x - Checkbox.vSize.y,
                      Checkbox.vPos.y);
    RenderRect (vBoxPos, Vector2f (Checkbox.vSize.y, Checkbox.vSize.y),
                InsideColor, s_UnselectColor);

    if (IsWidgetSelected (Checkbox))
    {
        RenderRect (Checkbox.vPos, Checkbox.vSize,
                    sf::Color::Black, sf::Color::White);
    }

    return true;
}//RenderCheckbox


//RenderEditbox
bool Screen::RenderEditbox (const Widget &Editbox)
{
    sf::String Caption (Editbox.sText + L":", sf::Font::GetDefaultFont ());
    Caption.SetPosition (Editbox.vPos);
    Draw (Caption);

    wstring sValue = Editbox.sValue;

    if (IsWidgetSelected (Editbox))
    {
        if (m_bWidgetActive)
        {
            sValue += L"|";
        }
        RenderRect (Editbox.vPos, Editbox.vSize,
                    sf::Color::Black, sf::Color::White);
    }

    sf::String Value (sValue, sf::Font::GetDefaultFont ());
    Value.SetPosition (Caption.GetRect ().Right + s_nEditValueMargin,
                       Caption.GetRect ().Top - 2);
    Draw (Value);

    return true;
}//RenderEditbox


//RenderList
bool Screen::RenderList (const Widget &List)
{
    Err fErr (m_Err, L"RenderList");
    sf::String Caption (List.sText + L":", sf::Font::GetDefaultFont ());
    Caption.SetPosition (List.vPos);
    Draw (Caption);

    int nEntryId;
    if (!StrToNum (List.sValue, &nEntryId))
    {
        OPut (List.sValue);
        return fErr.Set (s_sErrListValue);
    }

    sf::Color BorderColor = s_UnselectColor;
    if (IsWidgetSelected (List))
    {
        if (m_bWidgetActive)
        {
            BorderColor = sf::Color::White;
        }
        RenderRect (List.vPos, List.vSize, sf::Color::Black, sf::Color::White);
    }

    Vector2f vValueSize;
    vValueSize.x = List.vSize.x -
                   Caption.GetRect ().GetWidth () - s_nListValueMargin;
    vValueSize.y = List.vSize.y;
    Vector2f vValuePos (Caption.GetRect ().Right + s_nListValueMargin,
                        Caption.GetRect ().Top);
    RenderRect (vValuePos, vValueSize, sf::Color::Black, BorderColor);

    sf::String Value (List.lsEntries.at (nEntryId),
                      sf::Font::GetDefaultFont ());
    Value.SetCenter (Value.GetRect ().GetWidth () / 2,
                     Value.GetRect ().GetHeight () / 2);
    Value.SetX (vValuePos.x + (vValueSize.x / 2));
    Value.SetY (vValuePos.y + (vValueSize.y / 2) - 3);
    Draw (Value);

    return true;
}//RenderList


//IsWidgetSelected
bool Screen::IsWidgetSelected (const Widget &fWidget)
{
    if (m_nSelectedWidget != -1 &&
        &GetCurWidget () == &fWidget &&
        fWidget.nId != 0)
    {
        return true;
    }
    return false;
}//IsWidgetSelected


//FillWidget
bool Screen::FillWidget (Widget *pWidget, enType nType,
                         wstring sText, Vector2f vPos)
{
    Err fErr (m_Err, L"FillWidget");
    if (pWidget == NULL || sText.empty ())
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    pWidget->nType = nType;
    pWidget->sText = sText;
    pWidget->vPos = vPos;
    pWidget->nId = 0;

    return true;
}//FillWidget


//RenderRect
void Screen::RenderRect (Vector2f vPos, Vector2f vSize,
                         const sf::Color &InsideColor,
                         const sf::Color &BorderColor)
{
    sf::Shape Rect;
    Rect = sf::Shape::Rectangle (vPos.x, vPos.y,
                                 vPos.x + vSize.x, vPos.y + vSize.y,
                                 InsideColor);
    Rect.SetOutlineWidth (2.5);
    Rect.SetPointOutlineColor (0, BorderColor);
    Rect.SetPointOutlineColor (1, BorderColor);
    Rect.SetPointOutlineColor (2, BorderColor);
    Rect.SetPointOutlineColor (3, BorderColor);
    Rect.SetBlendMode (sf::Blend::Add);

    Draw (Rect);
}//RenderRect


//GetWidgetCenter
bool Screen::GetWidgetCenter (const Widget &fWidget, Vector2f *pvCenter)
{
    Err fErr (m_Err, L"GetWidgetCenter");
    if (pvCenter == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    pvCenter->x = fWidget.vPos.x + (fWidget.vSize.x / 2);
    pvCenter->y = fWidget.vPos.y + (fWidget.vSize.y / 2);

    return true;
}//GetWidgetCenter


//GetWidgetRect
bool Screen::GetWidgetRect (const Widget &fWidget, sf::Rect<int> *pRect)
{
    Err fErr (m_Err, L"GetWidgetRect");
    if (pRect == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    pRect->Left = fWidget.vPos.x;
    pRect->Top = fWidget.vPos.y;
    pRect->Right = pRect->Left + fWidget.vSize.x;
    pRect->Bottom = pRect->Top + fWidget.vSize.y;

    return true;;
}//GetWidgetRect


//CheckSelectId
bool Screen::CheckSelectId ()
{
    Err fErr (m_Err, L"CheckSelectId");
    if (m_nSelectedWidget == -1)
    {
        return fErr.Set (L"A selected widget does not exists");
    }
    if (m_nSelectedWidget == static_cast<int> (m_lWidgets.size ()))
    {
        return fErr.Set (L"A widget with the given id does not exists");
    }

    return true;
}//CheckSelectId


//IsMouseOverWidget
bool Screen::IsMouseOverWidget (const Widget &fWidget)
{
    sf::Rect<int> Rect;
    if (!GetWidgetRect (fWidget, &Rect))
    {
        return false;
    }

    return Rect.Contains (m_vMousePos.x, m_vMousePos.y);
}//IsMouseOverWidget


//GetCurWidget
const Screen::Widget &Screen::GetCurWidget ()
{
    return m_lWidgets.at (m_nSelectedWidget);
}//GetCurWidget


//GetCharKey
int Screen::GetCharKey (sf::Key::Code nMin, sf::Key::Code nMax)
{
    //Its not a for loop, because the key codes are not increasing
    int i = nMin;
    while (true)
    {
        if (m_pWindow->GetInput ().IsKeyDown (static_cast<sf::Key::Code> (i)))
        {
            return i;
        }

        if (i == nMax)
        {
            break;
        }
        i++;
    }

    return -1;
}//GetCharKey
