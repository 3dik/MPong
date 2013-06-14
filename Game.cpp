#include "Game.hpp"

//Game
Game::Game ()
{
}//Game


//~Game
Game::~Game ()
{
    m_Window.Close ();

    if (m_Err.IsError ())
    {
        std::wcout<<L"Press \"enter\" to continue";
        std::cin.ignore ();
        std::wcout<<std::endl;
    }
}//Game


//Init
bool Game::Init (Err *pErr)
{
    if (!m_Err.SetClass (pErr, L"Game"))
    {
        return false;
    }
    Err fErr (m_Err, L"Init");

    srand (time (NULL));

    //Set locale
    #ifdef __unix__
    std::locale Locale ("");
    std::string sLocale = Locale.name ();

    #else
    #endif
    std::locale::global (std::locale (sLocale.c_str ()));


    if (!m_Config.Init (&m_Err, L"Config" + sFILEEXTENSION))
    {
        return false;
    }

    wstring sProfileDir;
    if (!m_Config.Get (L"ProfileDir", &sProfileDir))
    {
        return false;
    }
    if (!m_Profiles.Init (&m_Err, &m_Window, sProfileDir))
    {
        return false;
    }

    if (!m_Menu.Init (&m_Err, &m_Config, &m_Language, &m_Profiles, &m_Window))
    {
        return false;
    }

    return true;
}//Init


//Run
bool Game::Run ()
{
    bool bQuit = false;
    while (!bQuit)
    {
        if (!m_Menu.Run (&bQuit))
        {
            return false;
        }
        if (bQuit)
        {
            break;
        }

        Match NewMatch;
        if (!NewMatch.Init (&m_Err, &m_Config, &m_Language,
                            &m_Profiles, &m_Window, 100) ||
            !NewMatch.Run (&bQuit))
        {
            return false;
        }
    }

    return true;
}//Run
