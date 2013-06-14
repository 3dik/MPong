#include "Message.hpp"


//Init
bool Message::Init (Err *pErr, sf::RenderWindow *pWindow)
{
    if (!m_Err.SetClass (pErr, L"Message") ||
        !Render::Init (pErr, pWindow))
    {
        return false;
    }

    m_LastFrame = 0;

    return true;
}//Init


//Render
bool Message::Render ()
{
    bool bEnoughPlace = true;
    for (std::list<Line>::iterator it = m_lLines.begin ();
         it != m_lLines.end ();
         it++)
    {
        if (it->nPos > 1050)
        {
            m_lLines.erase (it);
            it = m_lLines.begin ();
            continue;
        }
        if (!bEnoughPlace)
        {
            break;
        }
        if (it->nPos < 80)
        {
            bEnoughPlace = false;
        }
        sf::String CurLine (it->sText, sf::Font::GetDefaultFont ());
        CurLine.SetPosition (500, it->nPos - 50);
        CurLine.SetCenter (CurLine.GetRect ().GetWidth () / 2,
                           CurLine.GetRect ().GetHeight () / 2);
        CurLine.SetColor (sf::Color (255, 255, 255, 128));
        it->nPos += m_Timer.GetElapsedTime () * 200;
        Draw (CurLine);
    }

    m_LastFrame = m_Timer.GetElapsedTime ();
    m_Timer.Reset ();

    return true;
}//Render


//Add
bool Message::Add (wstring sText)
{
    Err fErr (m_Err, L"Add");
    if (sText.empty ())
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    Line NewLine;
    NewLine.sText = sText;
    NewLine.nPos = 0;
    m_lLines.push_back (NewLine);

    return true;
}//Add


//Clear
void Message::Clear ()
{
    m_lLines.clear ();
}//Clear
