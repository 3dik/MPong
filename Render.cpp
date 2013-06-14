#include "Render.hpp"

Vector2f Render::s_vRes;

//SetRes
void Render::SetRes (Vector2f vRes)
{
    s_vRes = vRes;
}//SetRes


//GetRes
Vector2f &Render::GetRes ()
{
    return s_vRes;
}//GetRes


//Render
Render::Render ()
{
    m_pWindow = NULL;
}//Render


//Init
bool Render::Init (Err *pErr, sf::RenderWindow *pWindow)
{
    if (!m_Err.SetClass (pErr, L"Render"))
    {
        return false;
    }
    Err fErr (m_Err, L"Init");
    if (pWindow == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    m_pWindow = pWindow;

    return true;
}//Init
