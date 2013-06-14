#include "Line.hpp"


//Line
Line::Line ()
{
    m_nState = eActive;
}//Line


//Init
bool Line::Init (Err *pErr, sf::RenderWindow *pWindow,
                 Vector2f vBegin, Vector2f vEnd, bool bVisible)
{
    if (!m_Err.SetClass (pErr, L"Line") ||
        !Render::Init (&m_Err, pWindow))
    {
        return false;
    }

    m_vBegin = vBegin;
    m_vEnd = vEnd;
    m_bVisible = bVisible;

    return true;
}//Init


//Render
bool Line::Render () const
{
    if (m_nState == eInactive)
    {
        return true;
    }

    DrawLine (sf::Color::White);

    return true;
}//Render


//GetBegin
Vector2f Line::GetBegin () const
{
    return m_vBegin;
}//GetBegin


//GetEnd
Vector2f Line::GetEnd () const
{
    return m_vEnd;
}//GetEnd


//DrawLine
void Line::DrawLine (sf::Color Color) const
{
    if (!m_bVisible)
    {
        return;
    }
    sf::Shape Line = sf::Shape::Line (m_vBegin, m_vEnd, 2, Color);
    Draw (Line);
}//DrawLine
