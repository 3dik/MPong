#include "Shield.hpp"

const float Shield::s_nSize = 50;

std::vector<Goal> *Shield::s_plGoals = NULL;
unsigned int       Shield::s_nSpeed = 0;
Message           *Shield::s_pMessage = NULL;
wstring            Shield::s_sJoined;

//Shield
Shield::Shield ()
{
    m_pProfile = NULL;

    m_nState    = eInactive;
    m_nMovement = eStop;
}//Shield


//Init
bool Shield::Init (Err *pErr, sf::RenderWindow *pWindow, Profile *pProfile,
                   unsigned int nStartGoal)
{
    if (!m_Err.SetClass (pErr, L"Shield") ||
        !Line::Init (pErr, pWindow))
    {
        return false;
    }
    Err fErr (m_Err, L"Init");
    if (pProfile == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    if (s_plGoals == NULL)
    {
        return fErr.Set (L"There is no goal list");
    }

    m_nPosition = 0;
    m_pProfile = pProfile;
    m_nCurGoal = nStartGoal;
        m_nSpecialPressTime = 0;

    SetCoordinates ();
    CalcRange ();

    return true;
}//Init


//Move
bool Shield::Move ()
{
    bool bSpecial = m_pProfile->Special.IsPressed ();
    if (bSpecial && m_nState == eInactive)
    {
        m_nState = eActive;
        m_Timer.Reset ();
        Goal::AdjustFreeShieldCount (true);
        return true;
    }
    if (m_nState == eInactive || m_nState == eDead)
    {
        return true;
    }

    if (m_nState == eActive)
    {
        if (bSpecial)
        {
            m_nSpecialPressTime += m_Timer.GetElapsedTime ();
            if (m_nSpecialPressTime > 1)
            {
                Fixate ();
            }
        }
        else
        {
            m_nSpecialPressTime -= m_Timer.GetElapsedTime ();
            if (m_nSpecialPressTime < 0)
            {
                m_nSpecialPressTime = 0;
            }
        }
        m_Timer.Reset ();
    }

    bool bFurther = m_pProfile->Right.IsPressed ();
    bool bBack = m_pProfile->Left.IsPressed ();
    m_pProfile->Left.ResetMouse ();
    m_pProfile->Right.ResetMouse ();
    m_pProfile->Special.ResetMouse ();

    if (bSpecial)
    {
        m_nMovement = eStop;
    }
    else if (bFurther)
    {
        SetDirection (true);
        //m_nMovement = eFurther;
    }
    else if (bBack)
    {
        SetDirection (false);
        //m_nMovement = eBack;
    }
    if (m_nMovement == eStop)
    {
        return true;
    }

    short nDir = 1;
    if (m_nMovement == eBack)
    {
        nDir = -1;
    }
    m_nPosition += nDir * m_pWindow->GetFrameTime () * s_nSpeed;
    if (m_nPosition - s_nSize < -m_nMaxRange)
    {
        if (m_nState == eActive)
        {
            SwitchGoal (false);
        }
        else
        {
            m_nMovement = eStop;
            MoveToEnd (true);
        }
    }
    else if (m_nPosition + s_nSize > m_nMaxRange)
    {
        if (m_nState == eActive)
        {
            SwitchGoal (true);
        }
        else
        {
            m_nMovement = eStop;
            MoveToEnd (false);
        }
    }
    SetCoordinates ();

    return true;
}//Move


//SetGoalList
void Shield::SetGoalList (std::vector<Goal> *plGoals)
{
    s_plGoals = plGoals;
}//SetGoalList


//SetSpeed
void Shield::SetSpeed (unsigned int nSpeed)
{
    s_nSpeed = nSpeed;
}//SetSpeed


//SetMessage
void Shield::SetMessage (Message *pMessage)
{
    s_pMessage = pMessage;
}//SetMessage


//SetLanguage
bool Shield::SetLanguage (Config *pLanguage)
{
    if (!pLanguage->Get (L"PlayJoined", &s_sJoined))
    {
        return false;
    }
    return true;
}//SetLanguage


//SetCoordinates
void Shield::SetCoordinates ()
{
    m_vBegin = s_plGoals->at (m_nCurGoal).GetBegin ();
    m_vEnd = s_plGoals->at (m_nCurGoal).GetEnd ();
    Vector2f vRel (m_vEnd.x - m_vBegin.x,
                   m_vEnd.y - m_vBegin.y);
    Vector2f vDir = Normalize (vRel);
    Vector2f vMiddle (m_vBegin.x + vRel.x / 2,
                      m_vBegin.y + vRel.y / 2);
    m_vBegin.x = vMiddle.x - ((s_nSize - m_nPosition) * vDir.x);
    m_vBegin.y = vMiddle.y - ((s_nSize - m_nPosition) * vDir.y);
    m_vEnd.x = vMiddle.x + ((s_nSize + m_nPosition) * vDir.x);
    m_vEnd.y = vMiddle.y + ((s_nSize + m_nPosition) * vDir.y);
}//SetCoordinates


//CalcRange
void Shield::CalcRange ()
{
    Vector2f vBegin = s_plGoals->at (m_nCurGoal).GetBegin ();
    Vector2f vEnd = s_plGoals->at (m_nCurGoal).GetEnd ();
    Vector2f vRel (vEnd.x - vBegin.x,
                   vEnd.y - vBegin.y);
    m_nMaxRange = Distance (vRel) / 2;
}//CalcRange


//SwitchGoal
void Shield::SwitchGoal (bool bFurther)
{
    int nDir = 1;
    if (!bFurther)
    {
        nDir = -1;
    }

    m_nCurGoal += nDir;

    if (m_nCurGoal < 0)
    {
        m_nCurGoal = s_plGoals->size () - 1;
    }
    else if (static_cast<unsigned int> (m_nCurGoal) == s_plGoals->size ())
    {
        m_nCurGoal = 0;
    }
    CalcRange ();
    MoveToEnd (bFurther);

}//SwitchGoal


//MoveToEnd
void Shield::MoveToEnd (bool bBegin)
{
    int nDir = 1;
    if (bBegin)
    {
        nDir = -1;
    }
    m_nPosition = (nDir * m_nMaxRange) - (nDir * s_nSize);
}//MoveToEnd


//Fixate
void Shield::Fixate ()
{
    m_nState = ePlay;
    s_plGoals->at (m_nCurGoal).AdjustShieldCount (true);
    Goal::AdjustFreeShieldCount (false);

    wstring sName = m_pProfile->ProfileConfig.GetFileName ();
    sName = sName.substr (0, sName.find (sFILEEXTENSION));
    s_pMessage->Add (s_sJoined + L": " + sName);
}//Fixate


//SetDirection
void Shield::SetDirection (bool bFurther)
{
    if (!s_plGoals->at (m_nCurGoal).GetDirection ())
    {
        bFurther = !bFurther;
    }
    m_nMovement = eBack;
    if (bFurther)
    {
        m_nMovement = eFurther;
    }
}//SetDirection
