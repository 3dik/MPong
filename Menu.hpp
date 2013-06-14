//Manages the main menu

#pragma once

#include <map>
#include "SFML/Graphics.hpp"

#include "Config.hpp"
#include "Err.hpp"
#include "Functions.hpp"
#include "Key.hpp"
#include "Profiles.hpp"
#include "Render.hpp"
#include "Screen.hpp"


class Menu : public Render
{
    public:

    Menu ();
    ~Menu ();
    //Initializes
    bool Init (Err *pErr, Config *pConfig, Config *pLanguage,
               Profiles *pProfiles, sf::RenderWindow *pWindow);
    //Runs the menu, returns if the user starts a game or quits the
    bool Run (bool *pbQuit);

    private:

    //Chooses the right screen contingent on selection id
    bool ChooseScreen (bool *pbQuit);
    //Prepares the game start
    bool PrepareGameStart ();
    //Manages the screens of the profiles
    bool ManageProfileScreens ();
    //Initializes the profiles overview screen
    bool InitProfilesScreen (Screen *pScreen);
    //Runs a screen and wait for an user input and returns it
    bool AwaitInput (wstring *psKey);
    //Runs the screen of configuration for the given profile
    bool RunEditProfile (wstring sProfile, bool *pbModified);
    //Adds a profile, runs the profile edit screen und reload the profile system
    bool AddProfile (wstring sName);
    //Applies the values of the settings menu
    bool ApplySettings ();
    //Reinitializes
    bool Reinit ();
    //Initializes the main menu
    bool InitMenuMain ();
    //Initializes the settings menu
    bool InitMenuSettings ();
    //Initializes the profiles menu
    bool InitMenuProfiles ();
    //Initializes the about menu
    bool InitMenuAbout ();
    //Initializes the game menu
    bool InitMenuGame ();
    //Sets the video mode, resolution etc.
    bool SetWindow ();

    Err               m_Err;
    Config           *m_pConfig;
    Config           *m_pLanguage;
    Profiles         *m_pProfiles;
    Screen            m_ScreenMain;
    Screen            m_ScreenAbout;
    Screen            m_ScreenSettings;
    Screen            m_ScreenGame;
    Screen           *m_pCurrentScreen;
    bool              m_bClose;
    wstring           m_sAbort;
    wstring           m_sLanguageDir;
    wstring           m_sLevelDir;

    const static wstring s_sFramelimit;
    const static wstring s_sLanguageFile;
    const static wstring s_sVerticalSync;
    const static wstring s_sResDelim;
    const static wstring s_sResolution;
    const static wstring s_sColorDepth;
    const static wstring s_sMouseSensitivity;
    const static wstring s_sAntialiasing;

    const static wstring s_sErrResolution;

    //The widget id's
    enum
    {
        eInvalid,
        eMainButtonStart,
        eMainButtonSettings,
        eMainButtonAbout,
        eMainButtonProfiles,
        eMainButtonQuit,
        eGameButtonAbort,
        eGameButtonPlay,
        eGameListLevels,
        eSettingsCheckFullscreen,
        eSettingsCheckVerticalSync,
        eSettingsEditFramelimit,
        eSettingsEditAntialiasing,
        eSettingsListResolution,
        eSettingsListColorDepth,
        eSettingsListLanguage,
        eSettingsCheckHelp,
        eSettingsEditMouseSensitivity,
        eSettingsButtonAbort,
        eSettingsButtonAccept,
        eSettingsButtonOk,
        eProfilesListChoice,
        eProfilesButtonAbort,
        eProfilesButtonEdit,
        eProfilesButtonDelete,
        eProfilesButtonCreate,
        eProfilesEditCreateName,
        eProfileEditButtonAbort,
        eProfileEditButtonLeft,
        eProfileEditButtonRight,
        eProfileEditButtonSpecial,
        eProfileEditButtonOk,
        eAboutButtonAbort
    };
};
