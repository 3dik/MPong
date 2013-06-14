#include "Menu.hpp"

const wstring Menu::s_sFramelimit = L"FrameLimit";
const wstring Menu::s_sLanguageFile = L"LanguageFile";
const wstring Menu::s_sVerticalSync = L"VerticalSync";
const wstring Menu::s_sResDelim = L" x ";
const wstring Menu::s_sResolution = L"WindowResolution";
const wstring Menu::s_sColorDepth = L"WindowDepth";
const wstring Menu::s_sMouseSensitivity = L"MouseSensitivity";
const wstring Menu::s_sAntialiasing = L"Antialiasing";

const wstring Menu::s_sErrResolution = L"The given resolution is invalid";


//Menu
Menu::Menu ()
{
    m_pConfig = NULL;
    m_pLanguage = NULL;
    m_pProfiles = NULL;
    m_pWindow = NULL;
    m_pCurrentScreen = NULL;
}//Menu


//~Menu
Menu::~Menu ()
{
}//~Menu


//Init
bool Menu::Init (Err *pErr, Config *pConfig, Config *pLanguage,
                 Profiles *pProfiles, sf::RenderWindow *pWindow)
{
    if (!m_Err.SetClass (pErr, L"Menu") ||
        !Render::Init (&m_Err, pWindow))
    {
        return false;
    }
    Err fErr (m_Err, L"Init");
    if (pConfig == NULL || pLanguage == NULL ||
        pProfiles == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    m_pConfig = pConfig;
    m_pLanguage = pLanguage;
    m_pProfiles = pProfiles;

    if (!m_pConfig->Get (sCONF_LEVELDIR, &m_sLevelDir) ||
        !m_pConfig->Get (L"LanguageDir", &m_sLanguageDir))
    {
        return false;
    }
    m_sLevelDir += L"/";
    m_sLanguageDir += L"/";

    m_pCurrentScreen = &m_ScreenMain;

    return Reinit ();
}//Init


//Run
bool Menu::Run (bool *pbQuit)
{
    Err fErr (m_Err, L"Run");
    if (pbQuit == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    *pbQuit = true;
    m_bClose = false;

    while (!m_bClose)
    {
        if (!m_pCurrentScreen->Run ())
        {
            return false;
        }
        if (m_pCurrentScreen->IsQuit ())
        {
            m_bClose = true;
            *pbQuit = true;
            break;
        }

        Screen *pPreviousScreen = m_pCurrentScreen;
        if (!ChooseScreen (pbQuit) ||
            !pPreviousScreen->Reset ())
        {
            return false;
        }
    }

    return true;
}//Run


//ChooseScreen
bool Menu::ChooseScreen (bool *pbQuit)
{
    Err fErr (m_Err, L"ChooseScreen");
    if (pbQuit == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    if (m_pCurrentScreen == NULL)
    {
        return fErr.Set (L"The pointer for the current screen is invalid");
    }

    unsigned int nSelected;
    if (!m_pCurrentScreen->GetSelectId (&nSelected))
    {
        return false;
    }

    switch (nSelected)
    {
        case (eMainButtonStart):
            m_pCurrentScreen = &m_ScreenGame;
            break;
        case (eGameButtonPlay):
            if (!PrepareGameStart ())
            {
                return false;
            }
            *pbQuit = false;
            break;
        case (eMainButtonQuit):
            m_bClose = true;
            break;
        case (eMainButtonSettings):
            m_pCurrentScreen = &m_ScreenSettings;
            break;
        case (eMainButtonProfiles):
            if (!ManageProfileScreens ())
            {
                return false;
            }
            break;
        case (eMainButtonAbout):
            m_pCurrentScreen = &m_ScreenAbout;
            break;
        case (eGameButtonAbort):
        case (eAboutButtonAbort):
        case (eSettingsButtonAbort):
            m_pCurrentScreen = &m_ScreenMain;
            break;
        case (eSettingsButtonOk):
        case (eSettingsButtonAccept):
            {
                if (!ApplySettings () ||
                    !Reinit () ||
                    !m_pConfig->Save ())
                {
                    return false;
                }

                if (nSelected == eSettingsButtonOk)
                {
                    m_pCurrentScreen = &m_ScreenMain;
                }
                break;
            }
        default:
            return fErr.Set (L"The given selectection id is unknown: " +
                             NumToStr (nSelected));
    }

    return true;
}//ChooseScreen


//PrepareGameStart
bool Menu::PrepareGameStart ()
{
    wstring sLevel;
    if (!m_pCurrentScreen->Get (eGameListLevels, &sLevel))
    {
        return false;
    }
    m_pConfig->Set (sCONF_LEVELFILE, sLevel + sFILEEXTENSION);

    if (!m_pConfig->Save () ||
        !InitMenuGame ())
    {
        return false;
    }

    m_bClose = true;

    return true;
}//PrepareGameStart


//ManageProfileScreens
bool Menu::ManageProfileScreens ()
{
    Err fErr (m_Err, L"ManageProfileScreens");

    Screen Overview;
    if (!InitProfilesScreen (&Overview))
    {
        return false;
    }

    unsigned int nAction;
    do
    {
        if (!Overview.Run ())
        {
            return false;
        }
        if (Overview.IsQuit ())
        {
            m_bClose = true;
            break;
        }
        if (!Overview.GetSelectId (&nAction))
        {
            return false;
        }

        wstring sName;
        if (!Overview.Get (eProfilesListChoice, &sName))
        {
            return false;
        }

        switch (nAction)
        {
            case (eProfilesButtonEdit):
                {
                    bool bModified;
                    if (!RunEditProfile (sName, &bModified))
                    {
                        return false;
                    }
                    if (bModified && !m_pProfiles->Reload ())
                    {
                        return false;
                    }
                }
                break;
            case (eProfilesButtonCreate):
                {
                    wstring sName;
                    if (!Overview.Get (eProfilesEditCreateName, &sName))
                    {
                        return false;
                    }
                    if (sName.empty ())
                    {
                        fErr.Set (L"The given profile name is empty");
                    }
                    if (!AddProfile (sName) ||
                        !InitProfilesScreen (&Overview))
                    {
                        return false;
                    }
                }
                break;
            case (eProfilesButtonDelete):
                if (!m_pProfiles->DeleteProfile (sName) ||
                    !InitProfilesScreen (&Overview))
                {
                    return false;
                }
                break;
        }

        if (!Overview.Reset ())
        {
            return false;
        }
    } while (!m_bClose && nAction != eProfilesButtonAbort);

    return true;
}//ManageProfileScreens


//InitProfilesScreen
bool Menu::InitProfilesScreen (Screen *pScreen)
{
    Err fErr (m_Err, L"InitProfilesScreen");
    if (pScreen == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    wstring sMenuProfiles, sList, sEdit, sDelete,
            sCreate, sCreateName, sStandard;
    if (!m_pLanguage->Get (L"ProfilesTitle", &sMenuProfiles) ||
        !m_pLanguage->Get (L"ProfilesList", &sList) ||
        !m_pLanguage->Get (L"ProfilesEdit", &sEdit) ||
        !m_pLanguage->Get (L"ProfilesDelete", &sDelete) ||
        !m_pLanguage->Get (L"ProfilesCreate", &sCreate) ||
        !m_pLanguage->Get (L"ProfilesCreateName", &sCreateName) ||
        !m_pLanguage->Get (L"ProfileStandard", &sStandard))
    {
        return false;
    }

    if (!pScreen->Init (&m_Err, m_pProfiles, m_pWindow, sMenuProfiles))
    {
        return false;
    }
    std::vector<Profile> *plProfiles;
    if (!m_pProfiles->GetList (&plProfiles))
    {
        return false;
    }
    if (plProfiles->size () == 0 && !AddProfile (sStandard))
    {
        return false;
    }
    std::vector<wstring> lsProfileList;
    for (unsigned int i = 0; i != plProfiles->size (); i++)
    {
        wstring sName;
        if (!m_pProfiles->GetName (i, &sName))
        {
            return false;
        }
        lsProfileList.push_back (sName);
    }
    if (!pScreen->AddButton (sEdit, Vector2f (100, 190), eProfilesButtonEdit) ||
        !pScreen->AddList (sList, Vector2f (350, 220), lsProfileList, 0,
                           eProfilesListChoice) ||
        !pScreen->AddButton (sDelete, Vector2f (100, 250),
                             eProfilesButtonDelete) ||
        !pScreen->AddButton (sCreate, Vector2f (100, 400),
                             eProfilesButtonCreate) ||
        !pScreen->AddEditbox (sCreateName, Vector2f (350, 400), 300,
                              eProfilesEditCreateName) ||
        !pScreen->AddButton (m_sAbort, Vector2f (700, 600),
                             eProfilesButtonAbort))
    {
        return false;
    }

    return true;
}//InitProfilesScreen


//AwaitInput
bool Menu::AwaitInput (wstring *psKey)
{
    Err fErr (m_Err, L"AwaitInput");
    if (psKey == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    psKey->clear ();

    wstring sInfo;
    if (!m_pLanguage->Get (L"KeyInfo", &sInfo))
    {
        return false;
    }

    Key MouseUp, MouseDown, MouseLeft, MouseRight;
    if (!MouseUp.Init (&m_Err, m_pWindow, sMOUSE_UP, false) ||
        !MouseDown.Init (&m_Err, m_pWindow, sMOUSE_DOWN, false) ||
        !MouseLeft.Init (&m_Err, m_pWindow, sMOUSE_LEFT, false) ||
        !MouseRight.Init (&m_Err, m_pWindow, sMOUSE_RIGHT, false))
    {
        return false;
    }

    while (psKey->empty () && !m_bClose)
    {
        sf::Event Event;
        while (m_pWindow->GetEvent (Event))
        {
            switch (Event.Type)
            {
                case (sf::Event::Closed):
                    m_bClose = true;
                    break;
                case (sf::Event::KeyPressed):
                    if (Event.Key.Code == sf::Key::Escape)
                    {
                        return true;
                    }
                    *psKey = Key::GetInputName (*m_pWindow, Event.Key.Code);
                    break;
                case (sf::Event::MouseButtonPressed):
                    *psKey = Key::GetInputName (*m_pWindow, Event.Key.Code,
                                                Key::eMouse);
                    break;
                case (sf::Event::MouseMoved):
                    if (MouseDown.IsPressed ())
                    {
                        *psKey = sMOUSE_DOWN;
                    }
                    if (MouseUp.IsPressed ())
                    {
                        *psKey = sMOUSE_UP;
                    }
                    if (MouseLeft.IsPressed ())
                    {
                        *psKey = sMOUSE_LEFT;
                    }
                    if (MouseRight.IsPressed ())
                    {
                        *psKey = sMOUSE_RIGHT;
                    }
                    break;
                default:
                    break;
            }
        }

        m_pWindow->Clear ();

        sf::String Info (sInfo, sf::Font::GetDefaultFont ());
        Info.SetCenter (Info.GetRect ().GetWidth () / 2,
                        Info.GetRect ().GetHeight () / 2);
        Info.SetPosition (500, 400);
        Draw (Info);

        m_pWindow->Display ();

        MouseUp.ResetMouse ();
        MouseDown.ResetMouse ();
        MouseLeft.ResetMouse ();
        MouseRight.ResetMouse ();

        sf::Sleep (nSLEEPTIME);
    }

    return true;
}//AwaitInput


//RunEditProfile
bool Menu::RunEditProfile (wstring sName, bool *pbModified)
{
    Err fErr (m_Err, L"RunEditProfile");
    if (sName.empty () || pbModified == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    *pbModified = false;

    Config *pProfile;
    bool bCreated;
    if (!m_pProfiles->GetByName (sName, &pProfile, &bCreated))
    {
        return false;
    }

    wstring sMenuEdit, sLeft, sRight, sSpecial, sSet, sOk, sUndefined;
    if (!m_pLanguage->Get (L"ProfileEditTitle", &sMenuEdit) ||
        !m_pLanguage->Get (L"ProfileEditLeft", &sLeft) ||
        !m_pLanguage->Get (L"ProfileEditRight", &sRight) ||
        !m_pLanguage->Get (L"ProfileEditSpecial", &sSpecial) ||
        !m_pLanguage->Get (L"ProfileEditSet", &sSet) ||
        !m_pLanguage->Get (L"ProfileEditOk", &sOk) ||
        !m_pLanguage->Get (L"ProfileEditUndefined", &sUndefined))
    {
        return false;
    }
    sLeft += L": ";
    sRight += L": ";
    sSpecial += L": ";

    wstring sLeftVal, sRightVal, sSpecialVal;
    if (!pProfile->Get (sKEY_LEFT, &sLeftVal) ||
        !pProfile->Get (sKEY_RIGHT, &sRightVal) ||
        !pProfile->Get (sKEY_SPECIAL, &sSpecialVal))
    {
        return false;
    }

    unsigned int nAction;
    do
    {
        if (sLeftVal.empty ())
        {
            sLeftVal = sUndefined;
        }
        if (sRightVal.empty ())
        {
            sRightVal = sUndefined;
        }
        if (sSpecialVal.empty ())
        {
            sSpecialVal = sUndefined;
        }
        Screen EditScreen;
        if (!EditScreen.Init (&m_Err, m_pProfiles, m_pWindow, sMenuEdit + sName) ||
            !EditScreen.AddLabel (sLeft + sLeftVal, Vector2f (300, 150)) ||
            !EditScreen.AddButton (sSet, Vector2f (50, 140),
                                   eProfileEditButtonLeft) ||
            !EditScreen.AddLabel (sRight + sRightVal, Vector2f (300, 250)) ||
            !EditScreen.AddButton (sSet, Vector2f (50, 240),
                                   eProfileEditButtonRight) ||
            !EditScreen.AddLabel (sSpecial + sSpecialVal,
                                  Vector2f (300, 350)) ||
            !EditScreen.AddButton (sSet, Vector2f (50, 340),
                                   eProfileEditButtonSpecial) ||
            !EditScreen.AddButton (m_sAbort, Vector2f (700, 540),
                                   eProfileEditButtonAbort) ||
            !EditScreen.AddButton (sOk, Vector2f (700, 600),
                                   eProfileEditButtonOk))
        {
            return false;
        }
        if (!EditScreen.Run ())
        {
            return false;
        }
        if (EditScreen.IsQuit ())
        {
            m_bClose = true;
            break;
        }
        if (!EditScreen.GetSelectId (&nAction))
        {
            return false;
        }

        wstring *psCurKey = NULL;
        switch (nAction)
        {
            case (eProfileEditButtonLeft):
                psCurKey = &sLeftVal;
                break;
            case (eProfileEditButtonRight):
                psCurKey = &sRightVal;
                break;
            case (eProfileEditButtonSpecial):
                psCurKey = &sSpecialVal;
                break;
            case (eProfileEditButtonOk):
                pProfile->Set (sKEY_LEFT, sLeftVal);
                pProfile->Set (sKEY_RIGHT, sRightVal);
                pProfile->Set (sKEY_SPECIAL, sSpecialVal);
                if (!pProfile->Save ())
                {
                    return false;
                }
                *pbModified = true;
                nAction = eProfileEditButtonAbort;
                break;
        }

        if (psCurKey != NULL)
        {
            wstring sCode;
            if (!AwaitInput (&sCode))
            {
                return false;
            }
            if (!sCode.empty ())
            {
                *psCurKey = sCode;
            }
        }
    } while (!m_bClose && nAction != eProfileEditButtonAbort);

    return true;
}//RunEditProfile


//AddProfile
bool Menu::AddProfile (wstring sName)
{
    Err fErr (m_Err, L"AddProfile");
    if (sName.empty ())
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    bool bModified;
    if (!m_pProfiles->AddProfile (sName + sFILEEXTENSION) ||
        !RunEditProfile (sName, &bModified))
    {
        return false;
    }

    if (bModified && !m_pProfiles->Reload ())
    {
        return false;
    }

    return true;
}//AddProfile


//ApplySettings
bool Menu::ApplySettings ()
{
    Err fErr (m_Err, L"ApplySettings");

    bool bFullscreen, bVerticalSync, bHelp;
    wstring sFramelimit, sLanguageFile, sResolution, sDepth, sMouseSensitivity,
            sAntialiasing;
    if (!m_pCurrentScreen->GetChecked (eSettingsCheckFullscreen,
                                       &bFullscreen) ||
        !m_pCurrentScreen->GetChecked (eSettingsCheckVerticalSync,
                                       &bVerticalSync) ||
        !m_pCurrentScreen->GetChecked (eSettingsCheckHelp, &bHelp) ||
        !m_pCurrentScreen->Get (eSettingsEditFramelimit, &sFramelimit) ||
        !m_pCurrentScreen->Get (eSettingsListLanguage, &sLanguageFile) ||
        !m_pCurrentScreen->Get (eSettingsListResolution, &sResolution) ||
        !m_pCurrentScreen->Get (eSettingsListColorDepth, &sDepth) ||
        !m_pCurrentScreen->Get (eSettingsEditMouseSensitivity,
                                &sMouseSensitivity) ||
        !m_pCurrentScreen->Get (eSettingsEditAntialiasing, &sAntialiasing))
    {
        return false;
    }

    if (!IsNumeric (sFramelimit))
    {
        fErr.Set (L"The given frame limit is invalid: " + sFramelimit);
    }
    else
    {
        m_pConfig->Set (s_sFramelimit, sFramelimit);
    }
    if (!IsNumeric (sDepth))
    {
        fErr.Set (L"The given color depth is invalid: " + sDepth);
    }
    else
    {
        m_pConfig->Set (s_sColorDepth, sDepth);
    }
    if (!IsNumeric (sMouseSensitivity))
    {
        fErr.Set (L"The given mouse sensitivity is invalid: " +
                  sMouseSensitivity);
    }
    else
    {
        m_pConfig->Set (s_sMouseSensitivity, sMouseSensitivity);
    }
    if (!IsNumeric (sAntialiasing))
    {
        fErr.Set (L"The given antialiasing level is invalid: " + sAntialiasing);
    }
    else
    {
        m_pConfig->Set (s_sAntialiasing, sAntialiasing);
    }

    m_pConfig->SetBool (sCONF_FULLSCREEN, bFullscreen);
    m_pConfig->SetBool (sCONF_HELP, bHelp);
    m_pConfig->SetBool (s_sVerticalSync, bVerticalSync);
    m_pConfig->Set (s_sLanguageFile, sLanguageFile);
    m_pConfig->Set (s_sResolution, sResolution);

    return true;
}//ApplySettings

//Reinit
bool Menu::Reinit ()
{
    Err fErr (m_Err, L"Reinit");

    wstring sLangFile;
    if (!m_pConfig->Get (s_sLanguageFile, &sLangFile))
    {
        return false;
    }
    if (!m_pLanguage->Init (&m_Err, m_sLanguageDir + sLangFile))
    {
        return false;
    }

    if (!SetWindow ())
    {
        return false;
    }

    if (!m_pLanguage->Get (L"Abort", &m_sAbort))
    {
        return false;
    }

    if (!InitMenuMain () ||
        !InitMenuSettings () ||
        !InitMenuAbout () ||
        !InitMenuGame ())
    {
        return false;
    }

    int nMouseSensitivity;
    if (!m_pConfig->GetNum (s_sMouseSensitivity, &nMouseSensitivity) ||
        !Key::SetMouseSensitivity (nMouseSensitivity))
    {
        return fErr.Set (L"Invalid mouse sensitivity");
    }

    m_bClose = false;

    return true;
}//Reinit


//InitMenuMain
bool Menu::InitMenuMain ()
{
    wstring sMenuMain, sStartgame, sAbout, sSettings, sQuit, sProfiles;
    if (!m_pLanguage->Get (L"MainTitle", &sMenuMain) ||
        !m_pLanguage->Get (L"MainStart", &sStartgame) ||
        !m_pLanguage->Get (L"MainAbout", &sAbout) ||
        !m_pLanguage->Get (L"MainSettings", &sSettings) ||
        !m_pLanguage->Get (L"MainProfiles", &sProfiles) ||
        !m_pLanguage->Get (L"MainQuit", &sQuit))
    {
        return false;
    }

    if (!m_ScreenMain.Init (&m_Err, m_pProfiles, m_pWindow, sMenuMain) ||
        !m_ScreenMain.AddButton (sStartgame, Vector2f (400, 200),
                                 eMainButtonStart) ||
        !m_ScreenMain.AddButton (sSettings, Vector2f (400, 260),
                                 eMainButtonSettings) ||
        !m_ScreenMain.AddButton (sProfiles, Vector2f (400, 320),
                                 eMainButtonProfiles) ||
        !m_ScreenMain.AddButton (sAbout, Vector2f (400, 380),
                                 eMainButtonAbout) ||
        !m_ScreenMain.AddButton (sQuit, Vector2f (400, 440), eMainButtonQuit))
    {
        return false;
    }

    return true;
}//InitMenuMain


//InitMenuSettings
bool Menu::InitMenuSettings ()
{
    Err fErr (m_Err, L"InitMenuSettings");

    wstring sMenuSettings, sFullscreen, sVerticalSync, sFramelimit, sOk,
            sLanguage, sResolution, sColorDepth, sMouseSensitivity,
            sAntialiasing, sAccept, sHelp;
    bool bFullscreen, bVerticalSync, bHelp;
    int nFramelimit, nMouseSensitivity, nAntialiasing;
    if (!m_pLanguage->Get (L"SettingsTitle", &sMenuSettings) ||
        !m_pLanguage->Get (L"SettingsResolution", &sResolution) ||
        !m_pLanguage->Get (L"SettingsColorDepth", &sColorDepth) ||
        !m_pLanguage->Get (L"SettingsFullscreen", &sFullscreen) ||
        !m_pLanguage->Get (L"SettingsAntialiasing", &sAntialiasing) ||
        !m_pLanguage->Get (L"SettingsVerticalSync", &sVerticalSync) ||
        !m_pLanguage->Get (L"SettingsFrameLimit", &sFramelimit) ||
        !m_pLanguage->Get (L"SettingsLanguage", &sLanguage) ||
        !m_pLanguage->Get (L"SettingsHelp", &sHelp) ||
        !m_pLanguage->Get (L"SettingsMouseSensitivity", &sMouseSensitivity) ||
        !m_pLanguage->Get (L"SettingsOk", &sOk) ||
        !m_pLanguage->Get (L"SettingsAccept", &sAccept) ||
        !m_pConfig->GetBool (sCONF_FULLSCREEN, &bFullscreen) ||
        !m_pConfig->GetBool (s_sVerticalSync, &bVerticalSync) ||
        !m_pConfig->GetBool (sCONF_HELP, &bHelp) ||
        !m_pConfig->GetNum (s_sFramelimit, &nFramelimit) ||
        !m_pConfig->GetNum (s_sMouseSensitivity, &nMouseSensitivity) ||
        !m_pConfig->GetNum (s_sAntialiasing, &nAntialiasing))
    {
        return false;
    }

    std::vector<wstring> lsLanguages;
    if (!ListFiles (m_sLanguageDir, &lsLanguages))
    {
        return fErr.Set (L"Could not list the language files");
    }
    unsigned int nLanguageId = 0;
    for (; nLanguageId != lsLanguages.size (); nLanguageId++)
    {
        if (lsLanguages.at (nLanguageId) == m_pLanguage->GetFileName ())
        {
            break;
        }
    }

    std::vector<wstring> lsResolutions, lsDepths;
    unsigned int nOldColorDepth = 0, nCurResId = 0,
                 nCurDepthId = 0, nCurDepth = 0;
    bool bNextDepth = false;
    wstring sCurResolution;
    if (!m_pConfig->Get (s_sResolution, &sCurResolution) ||
        !m_pConfig->GetNum (s_sColorDepth, &nCurDepth))
    {
        return false;
    }
    for (unsigned int i = 0; i != sf::VideoMode::GetModesCount (); i++)
    {
        sf::VideoMode Mode = sf::VideoMode::GetMode (i);

        if (Mode.BitsPerPixel != nOldColorDepth)
        {
            if (nOldColorDepth != 0)
            {
                bNextDepth = true;
            }
            nOldColorDepth = Mode.BitsPerPixel;
            if (nCurDepth == nOldColorDepth)
            {
                nCurDepthId = lsDepths.size ();
            }
            lsDepths.push_back (NumToStr (nOldColorDepth));
        }
        //Because all resolutions are mutliply saved for every color depth
        if (bNextDepth)
        {
            continue;
        }

        wstring sEntry (NumToStr (Mode.Width) + s_sResDelim +
                        NumToStr (Mode.Height));
        if (sEntry == sCurResolution)
        {
            nCurResId = lsResolutions.size ();
        }
        lsResolutions.push_back (sEntry);
    }

    if (!m_ScreenSettings.Init (&m_Err, m_pProfiles, m_pWindow,
                                sMenuSettings) ||
        !m_ScreenSettings.AddList (sResolution, Vector2f (50, 150),
                                   lsResolutions, nCurResId,
                                   eSettingsListResolution) ||
        !m_ScreenSettings.AddList (sColorDepth, Vector2f (500, 150), lsDepths,
                                   nCurDepthId, eSettingsListColorDepth) ||
        !m_ScreenSettings.AddCheckbox (sFullscreen, Vector2f (50, 250),
                                       eSettingsCheckFullscreen, bFullscreen) ||
        !m_ScreenSettings.AddEditbox (sAntialiasing, Vector2f (500, 250), 50,
                                      eSettingsEditAntialiasing,
                                      NumToStr (nAntialiasing)) ||
        !m_ScreenSettings.AddCheckbox (sVerticalSync, Vector2f (50, 350),
                                       eSettingsCheckVerticalSync,
                                       bVerticalSync) ||
        !m_ScreenSettings.AddEditbox (sFramelimit, Vector2f (600, 350), 100,
                                      eSettingsEditFramelimit,
                                      NumToStr (nFramelimit)) ||
        !m_ScreenSettings.AddList (sLanguage, Vector2f (50, 450),
                                   lsLanguages, nLanguageId,
                                   eSettingsListLanguage) ||
        !m_ScreenSettings.AddCheckbox (sHelp, Vector2f (400, 450),
                                       eSettingsCheckHelp, bHelp) ||
        !m_ScreenSettings.AddEditbox (sMouseSensitivity, Vector2f (50, 550),
                                      150, eSettingsEditMouseSensitivity,
                                      NumToStr (nMouseSensitivity)) ||
        !m_ScreenSettings.AddButton (sOk, Vector2f (300, 650),
                                     eSettingsButtonOk) ||
        !m_ScreenSettings.AddButton (m_sAbort, Vector2f (525, 650),
                                     eSettingsButtonAbort) ||
        !m_ScreenSettings.AddButton (sAccept, Vector2f (750, 650),
                                     eSettingsButtonAccept))
    {
        return false;
    }

    return true;
}//InitMenuSettings


//InitMenuAbout
bool Menu::InitMenuAbout ()
{
    wstring sMenuAbout, sAboutGame;
    if (!m_pLanguage->Get (L"AboutTitle", &sMenuAbout) ||
        !m_pLanguage->Get (L"AboutGame", &sAboutGame))
    {
        return false;
    }

    if (!m_ScreenAbout.Init (&m_Err, m_pProfiles, m_pWindow, sMenuAbout) ||
        !m_ScreenAbout.AddLabel (sAboutGame, Vector2f (100, 200)) ||
        !m_ScreenAbout.AddButton (m_sAbort, Vector2f (700, 600),
                                  eAboutButtonAbort))
    {
        return false;
    }

    return true;
}//InitMenuAbout


//InitMenuGame
bool Menu::InitMenuGame ()
{
    Err fErr (m_Err, L"InitMenuGame");

    wstring sLevelDir, sMenuGame, sLevel, sPlay, sCurLevel;
    if (!m_pLanguage->Get (L"GameTitle", &sMenuGame) ||
        !m_pLanguage->Get (L"GameLevel", &sLevel) ||
        !m_pLanguage->Get (L"GamePlay", &sPlay) ||
        !m_pConfig->Get (sCONF_LEVELFILE, &sCurLevel))
    {
        return false;
    }

    std::vector<wstring> lsLevels;
    if (!ListFiles (m_sLevelDir, &lsLevels))
    {
        return fErr.Set (L"Could not list levels");
    }
    if (lsLevels.size () == 0)
    {
        return fErr.Set (L"There are no levels omg");
    }
    int nCurLevel = -1;
    for (unsigned int i = 0; i != lsLevels.size (); i++)
    {
        if (lsLevels.at (i) == sCurLevel)
        {
            nCurLevel = i;
        }
        if (!FilterExtension (lsLevels.at (i), &lsLevels.at (i)))
        {
            return fErr.Set (L"Could not extract the level name");
        }
    }
    if (nCurLevel == -1)
    {
        fErr.Set (L"The preset level file does not exists");
        nCurLevel = 0;
    }

    if (!m_ScreenGame.Init (&m_Err, m_pProfiles, m_pWindow, sMenuGame) ||
        !m_ScreenGame.AddList (sLevel, Vector2f (100, 150), lsLevels,
                               nCurLevel, eGameListLevels) ||
        !m_ScreenGame.AddButton (sPlay, Vector2f (700, 540), eGameButtonPlay) ||
        !m_ScreenGame.AddButton (m_sAbort, Vector2f (700, 600),
                                 eGameButtonAbort))
    {
        return false;
    }

    return true;
}//InitMenuGame


//SetWindow
bool Menu::SetWindow ()
{
    Err fErr (m_Err, L"SetWindow");

    int nDepth, nFrameLimit, nAntialiasing;
    if (!m_pConfig->GetNum (s_sColorDepth, &nDepth) ||
        !m_pConfig->GetNum (s_sFramelimit, &nFrameLimit) ||
        !m_pConfig->GetNum (s_sAntialiasing, &nAntialiasing))
    {
        return false;
    }

    unsigned int nWidth, nHeight;
    wstring sResolution;
    if (!m_pConfig->Get (s_sResolution, &sResolution))
    {
        return false;
    }
    size_t nDeliPos = sResolution.find (s_sResDelim);
    if (nDeliPos == wstring::npos ||
        !StrToNum (sResolution.substr (0, nDeliPos), &nWidth) ||
        !StrToNum (sResolution.substr (nDeliPos + s_sResDelim.size ()),
                                       &nHeight))
    {
        return fErr.Set (s_sErrResolution);
    }

    bool bFullscreen;
    if (!m_pConfig->GetBool (sCONF_FULLSCREEN, &bFullscreen))
    {
        return false;
    }
    unsigned long nWindowStyle = sf::Style::Resize | sf::Style::Close;
    if (bFullscreen)
    {
        nWindowStyle = nWindowStyle | sf::Style::Fullscreen;
    }

    m_pWindow->Create (sf::VideoMode (nWidth, nHeight, nDepth),
                       "MPong Developer Version", nWindowStyle,
                       sf::WindowSettings (24, 8, nAntialiasing));
    Render::SetRes (Vector2f (nWidth, nHeight));
    if (nFrameLimit != 0)
    {
        m_pWindow->SetFramerateLimit (nFrameLimit);
    }

    bool bVerticalSync;
    if (!m_pConfig->GetBool (s_sVerticalSync, &bVerticalSync))
    {
        return false;
    }
    m_pWindow->UseVerticalSync (bVerticalSync);

    return true;
}//SetWindow
