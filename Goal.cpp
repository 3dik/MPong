#include "Goal.hpp"

int Goal::s_nNumFixedGoals = 0;
int Goal::s_nNumFreeShields = 0;

//Goal
Goal::Goal ()
{
    m_nNumShields = 0;

    s_nNumFixedGoals = 0;
    s_nNumFreeShields = 0;
}//Goal


//Init
bool Goal::Init (Err *pErr, sf::RenderWindow *pWindow, bool bFurther,
                 Vector2f vBegin, Vector2f vEnd)
{
    if (!m_Err.SetClass (pErr, L"Goal") ||
        !Line::Init (pErr, pWindow, vBegin, vEnd))
    {
        return false;
    }

    m_bFurther = bFurther;
    return true;
}//Init


//Render
bool Goal::Render () const
{
    if (m_nNumShields == 0 && s_nNumFixedGoals > 1 && s_nNumFreeShields  == 0)
    {
        return Line::Render ();
    }

    DrawLine (sf::Color (255, 255, 255, 64));
    return true;
}//Render


//AdjustShieldCount
void Goal::AdjustShieldCount (bool bIncrease)
{
    if (bIncrease)
    {
        if (m_nNumShields == 0)
        {
            s_nNumFixedGoals ++;
        }
        m_nNumShields ++;
    }
    else
    {
        if (m_nNumShields == 1)
        {
            s_nNumFixedGoals --;
        }
        m_nNumShields --;
    }
}//AdjustShieldCount


//AdjustFreeShieldCount
void Goal::AdjustFreeShieldCount (bool bIncrease)
{
    if (bIncrease)
    {
        s_nNumFreeShields ++;
    }
    else
    {
        s_nNumFreeShields --;
    }
}//AdjustFreeShieldCount


//GetDirection
bool Goal::GetDirection ()
{
    return m_bFurther;
}//GetDirection
