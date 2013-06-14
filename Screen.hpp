//Manages a signle screen of the main menu

#pragma once

#include <vector>
#include <cmath>

#include "Convert.hpp"
#include "Err.hpp"
#include "Key.hpp"
#include "Misc.hpp"
#include "Profiles.hpp"
#include "Render.hpp"

using sf::Vector2f;

class Screen : public Render
{
    public:

    Screen ();
    ~Screen ();
    //Initializes
    bool Init (Err *pErr, Profiles *pProfiles,
               sf::RenderWindow *pWindow, wstring sCaption);
    //Adds a label
    bool AddLabel (wstring sCaption, Vector2f vPos);
    //Adds a button
    bool AddButton (wstring sCaption, Vector2f vPos, unsigned int nId = 0);
    //Adds a check box
    bool AddCheckbox (wstring sCaption, Vector2f vPos,
                      unsigned int nId = 0, bool bValue = false);
    //Adds a edit box
    bool AddEditbox (wstring sCaption, Vector2f vPos, unsigned int nSize,
                     unsigned int nId = 0, wstring sValue = L"");
    //Adds a list
    bool AddList (wstring sCaption, Vector2f vPos,
                  const std::vector<wstring> &lsEntries,
                  unsigned int nValue = 0, unsigned int nId = 0);
    //Runs until the user leaves the screen
    bool Run ();
    //Returns the value of a checkbox
    bool GetChecked (unsigned int nId, bool *pbValue);
    //Returns the value of an editbox or list
    bool Get (unsigned int nId, wstring *psValue);
    //Checks for "quit"
    bool IsQuit ();
    //Resets the state and all widgets
    bool Reset ();
    //Returns the last selected widget
    bool GetSelectId (unsigned int *pnId);

    private:

    enum enState
    {
        eRun,
        eStop,
        eQuit
    };
    enum enType
    {
        eLabel,
        eButton,
        eCheckbox,
        eEditbox,
        eList
    };
    struct Widget;

    //Adds or removes a editbox char
    bool AddEditValue (sf::Uint32 nChar);
    //Returns a pointer to a widget with the given id
    bool GetWidgetById (unsigned int nId, Widget **ppWidget);
    //Increases the selection id
    bool IncreaseSelectId (bool bNext);
    //Moves the mouse over a widget?
    bool MouseSelect ();
    //Manages user inputs
    bool ManageInputs ();
    //Executes the appropriate action for the selected widget
    bool ExecuteWidgetAction ();
    //User pressed a key for a "direction"
    bool Direction (bool bNext);
    //Increases the entry id of the current list
    bool IncreaseListEntryId (bool bNext);
    //Renders the screen
    bool Render ();
    //Renders the menu header
    bool RenderHeader ();
    //Renders a label
    bool RenderLabel (const Widget &Label);
    //Renders a button
    bool RenderButton (const Widget &Button);
    //Renders a checkbox
    bool RenderCheckbox (const Widget &Checkbox);
    //Renders an editbox
    bool RenderEditbox (const Widget &Editbox);
    //Renders a list
    bool RenderList (const Widget &List);
    //Is the given widget selected?
    bool IsWidgetSelected (const Widget &fWidget);
    //Fills a widget with given data
    bool FillWidget (Widget *pWidget, enType nType,
                     wstring sText, Vector2f vPos);
    //Renders a transparent rect with the given border color
    void RenderRect (Vector2f vPos, Vector2f vSize,
                     const sf::Color &InsideColor,
                     const sf::Color &BorderColor);
    //Returns the center of the given widget
    bool GetWidgetCenter (const Widget &fWidget, Vector2f *pvCenter);
    //Returns the rect of the given widget
    bool GetWidgetRect (const Widget &fWidget, sf::Rect<int> *pRect);
    //Checks the select id for validity
    bool CheckSelectId ();
    //Is mouse over the given widget
    bool IsMouseOverWidget (const Widget &fWidget);
    //Returns the selected widget
    const Widget &GetCurWidget ();
    //Returns the code of a char key if pressed
    int GetCharKey (sf::Key::Code nMin, sf::Key::Code nMax);

    unsigned char       m_nState;
    int                 m_nSelectedWidget;
    bool                m_bWidgetActive;
    wstring             m_sCaption;
    Err                 m_Err;
    Profiles           *m_pProfiles;
    std::vector<Widget> m_lWidgets;
    Vector2f            m_vMousePos;

    static bool s_bPressed;

    const static Vector2f  s_vButtonSize;
    const static int       s_nEditValueMargin;
    const static int       s_nListValueMargin;
    const static wstring   s_sErrWidgetType;
    const static wstring   s_sErrListValue;
    const static sf::Color s_UnselectColor;
};
