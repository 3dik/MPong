#include "Match.hpp"


//Match
Match::Match ()
{
    m_pConfig = NULL;
    m_pLanguage = NULL;
    m_pProfiles = NULL;
}//Match


//~Match ()
Match::~Match ()
{
}//~Match ()


//Init
bool Match::Init (Err *pErr, Config *pConfig, Config *pLanguage,
                  Profiles *pProfiles, sf::RenderWindow *pWindow,
                  int nFirstBalls)
{
    if (!m_Err.SetClass (pErr, L"Match") ||
        !Render::Init (&m_Err, pWindow))
    {
        return false;
    }
    Err fErr (m_Err, L"Init");
    if (pConfig == NULL || pLanguage == NULL || pProfiles == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    m_pConfig = pConfig;
    m_pLanguage = pLanguage;
    m_pProfiles = pProfiles;

    if(!LoadLevel () ||
       !LoadShields () ||
       !m_Message.Init (&m_Err, m_pWindow) ||
       !Shield::SetLanguage (m_pLanguage))
    {
        return false;
    }
    Shield::SetMessage (&m_Message);

    Ball::SetLines (&m_lWalls, &m_lShields, &m_lGoals);
    Ball::SetSpeed (75);
    Ball::SetRadius (5);

    m_nFirstBalls = nFirstBalls;

    return Reinit ();
}//Init


//Run
bool Match::Run (bool *pbQuit)
{
    Err fErr (m_Err, L"Run");
    if (pbQuit == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    m_bClose = false;
    *pbQuit = false;

    bool bFullscreen;
    if (!m_pConfig->GetBool (sCONF_FULLSCREEN, &bFullscreen))
    {
        return false;
    }
    if (bFullscreen)
    {
        m_pWindow->ShowMouseCursor (false);
    }

    while (!m_bClose && !*pbQuit)
    {
        sf::Event Event;
        while (m_pWindow->GetEvent (Event))
        {
            switch (Event.Type)
            {
                case (sf::Event::Closed):
                    *pbQuit = true;
                    break;
                case (sf::Event::KeyPressed):
                    if (Event.Key.Code == sf::Key::Escape)
                    {
                        m_bClose = true;
                    }
                    break;
                default:
                    break;
            }
        }

        if (!Render () ||
            !Move ())
        {
            return false;
        }
    }
    m_pWindow->ShowMouseCursor (true);

    sf::Sleep (nSLEEPTIME);

    return true;
}//Run


//Reinit
bool Match::Reinit ()
{
    m_Message.Clear ();

    bool bHelp;
    if (!m_pConfig->GetBool (sCONF_HELP, &bHelp))
    {
        return false;
    }

    if (bHelp)
    {
        wstring sHelpJoin, sHelpFix;
        if (!m_pLanguage->Get (L"PlayHelpJoin", &sHelpJoin) ||
            !m_pLanguage->Get (L"PlayHelpFix", &sHelpFix))
        {
            return false;
        }

        if (!m_Message.Add (sHelpFix) ||
            !m_Message.Add (sHelpJoin))
        {
            return false;
        }
    }

    m_lBalls.clear ();
    for (int i = 0; i != m_nFirstBalls; i++)
    {
        Ball NewBall;
        if (!NewBall.Init (&m_Err, m_pWindow, Vector2f (500, 375)))
        {
            return false;
        }
        m_lBalls.push_back (NewBall);
    }

    return true;
}//Reinit


//LoadLevel
bool Match::LoadLevel ()
{
    Err fErr (m_Err, L"LoadLevel");

    wstring sDir, sFile;
    if (!m_pConfig->Get (sCONF_LEVELDIR, &sDir) ||
        !m_pConfig->Get (sCONF_LEVELFILE, &sFile))
    {
        return false;
    }

    Config Level;
    if (!Level.Init (&m_Err, sDir + L"/" + sFile, true))
    {
        return false;
    }
    unsigned int nLineCount = 0;
    wstring svLastPos = L"0|0", sLastType =sLINE_FURTHER;
    while (true)
    {
        nLineCount ++;
        wstring sName, svBegin, svEnd, sType;
        sName = NumToStr (nLineCount) + L":";
        if (!Level.Get (sName + L"Begin", &svBegin) ||
            !Level.Get (sName + L"End", &svEnd) ||
            !Level.Get (sName + L"Type", &sType))
        {
            return false;
        }
        if (svBegin.empty () && svEnd.empty () && sType.empty ())
        {
            break;
        }
        //All arguments are optional. If they are undefined, the previous value
        //are set
        if (sType.empty ())
        {
            sType = sLastType;
        }
        if (svBegin.empty ())
        {
            svBegin = svLastPos;
        }
        if (svEnd.empty ())
        {
            svEnd = svLastPos;
        }
        if (svBegin == svEnd)
        {
            return fErr.Set (L"The begin and the end of the line are equal");
        }
        svLastPos = svEnd;
        sLastType = sType;

        Vector2f vBegin, vEnd;
        if (!ConvertVector (svEnd, &vEnd) ||
           (!ConvertVector (svBegin, &vBegin)))
        {
            return false;
        }

        bool bFurther;
        if (sType == sLINE_FURTHER)
        {
            bFurther = true;
        }
        else if (sType == sLINE_BACK)
        {
            bFurther = false;
        }
        else if (sType == sLINE_WALL)
        {
            if (!AddWall (vBegin, vEnd, true))
            {
                return false;
            }
            continue;
        }
        else
        {
            return fErr.Set (L"Invalid line type: " + sType);
        }
        Goal NewGoal;
        if (!NewGoal.Init (&m_Err, m_pWindow, bFurther, vBegin, vEnd))
        {
            return false;
        }
        m_lGoals.push_back (NewGoal);
    }
    if (m_lGoals.size () == 0)
    {
        return fErr.Set (L"There are no goals");
    }

    if (!AddWall (Vector2f (0, 0), Vector2f (1000, 0), false) ||
        !AddWall (Vector2f (1000, 0), Vector2f (1000, 750), false) ||
        !AddWall (Vector2f (1000, 750), Vector2f (0, 750), false) ||
        !AddWall (Vector2f (0, 750), Vector2f (0, 0), false))
    {
        return false;
    }

    return true;
}//LoadLevel


//AddWall
bool Match::AddWall (Vector2f vBegin, Vector2f vEnd, bool bVisible)
{
    Line NewWall;
    if (!NewWall.Init (&m_Err, m_pWindow, vBegin, vEnd, bVisible))
    {
        return false;
    }
    m_lWalls.push_back (NewWall);

    return true;
}//AddWall


//LoadShields
bool Match::LoadShields ()
{
    Shield::SetGoalList (&m_lGoals);
    Shield::SetSpeed (200);

    unsigned int nCurGoal = 0;
    std::vector<Profile> *plProfiles;
    if (!m_pProfiles->GetList (&plProfiles))
    {
        return false;
    }
    for (unsigned int nCurProfile = 0;
         nCurProfile != plProfiles->size ();
         nCurProfile++)
    {
        Shield NewShield;
        if (!NewShield.Init (&m_Err, m_pWindow,
                             &plProfiles->at (nCurProfile), nCurGoal))
        {
            return false;
        }
        m_lShields.push_back (NewShield);

        nCurGoal ++;
        if (nCurGoal == m_lGoals.size ())
        {
            nCurGoal = 0;
        }
    }

    return true;
}//LoadShields


//Render
bool Match::Render ()
{
    m_pWindow->Clear ();

    if (!RenderLineList (m_lGoals) ||
        !RenderLineList (m_lShields) ||
        !RenderLineList (m_lWalls) ||
        !m_Message.Render ())
    {
        return false;
    }

    for (std::list<Ball>::iterator it = m_lBalls.begin ();
         it != m_lBalls.end ();
         it ++)
    {
        if (!it->Render ())
        {
            return false;
        }
    }

    m_pWindow->Display ();

    return true;
}//Render


//Move
bool Match::Move ()
{
    for (unsigned int i = 0; i != m_lShields.size (); i++)
    {
        if (!m_lShields.at (i).Move ())
        {
            return false;
        }
    }

    for (std::list<Ball>::iterator it = m_lBalls.begin ();
         it != m_lBalls.end ();
         it ++)
    {
        if (!it->Move ())
        {
            return false;
        }
    }

    return true;
}//Move


//ConvertVector
bool Match::ConvertVector (wstring svPos, Vector2f *pvValue)
{
    Err fErr (m_Err, L"ConvertVector");
    if (svPos.empty () || pvValue == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    size_t nXLen = svPos.find (L"|");
    if (nXLen == wstring::npos)
    {
        return fErr.Set (L"Could not find the vector separator: " + svPos);
    }
    if (!StrToNum (svPos.substr (0, nXLen), &pvValue->x) ||
        !StrToNum (svPos.substr (nXLen + 1), &pvValue->y))
    {
        return fErr.Set (L"Invalid vector data: " + svPos);
    }

    return true;
}//ConvertVector
