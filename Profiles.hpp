//Manages the settings and inputs of the player profiles

#pragma once

#include "SFML/Graphics.hpp"
#include <vector>

#include "Config.hpp"
#include "Err.hpp"
#include "Functions.hpp"
#include "Key.hpp"
#include "Misc.hpp"


//Profile
struct Profile
{
    Config ProfileConfig;
    Key Special;
    Key Left;
    Key Right;
};//Profile


//User inputs
enum enInput
{
    eSpecial,
    eLeft,
    eRight
};//User inputs


class Profiles
{
    public:

    Profiles ();
    ~Profiles ();
    //Initializes
    bool Init (Err *pErr, sf::RenderWindow *pWindow, wstring sDirectory);
    //(Re)loads the profiles
    bool Reload ();
    //Returns the profile list
    bool GetList (std::vector<Profile> **ppList);
    //Is a binded input triggered?
    bool GetInput (enInput nInput, bool *pbState, bool bMouseMovement=true);
    //Is a binded input of the given profile triggered?
    bool GetInput (enInput nInput, bool *pbState,
                   unsigned int nProfileId, bool bMouseMovement=true);
    //Returns the name of the profile
    bool GetName (unsigned int nProfileId, wstring *psName);
    //Returns a pointer to the requested profile, the last arguments decides
    //whether a new profile will be created if a profile with the given name
    //does not exists.In this case the value pbCreate is "true".
    //If a creation is not allowed, the last parameter must be "NULL"
    bool GetByName (wstring sName, Config **ppProfile, bool *pbCreate = NULL);
    //Creates and saves a new profile and reloads the profile list
    bool AddProfile (wstring sFileName);
    //Deletes the given profiles and reloads the profile list
    bool DeleteProfile (wstring sName);

    private:

    //Checks the validity of the given profile id
    bool CheckProfileId (unsigned int nProfileId);

    Err                   m_Err;
    wstring               m_sDirectory;
    std::vector<Profile>  m_lProfiles;
    sf::RenderWindow     *m_pWindow;
};
