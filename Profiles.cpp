#include "Profiles.hpp"


//Profiles
Profiles::Profiles ()
{
    m_pWindow = NULL;
}//Profiles


//~Profiles
Profiles::~Profiles ()
{
}//~Profiles


//Init
bool Profiles::Init (Err *pErr, sf::RenderWindow *pWindow, wstring sDirectory)
{
    if (!m_Err.SetClass (pErr, L"Profiles"))
    {
        return false;
    }
    Err fErr (m_Err, L"Init");
    if (pWindow == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    m_pWindow = pWindow;
    m_sDirectory = sDirectory + L"/";

    return Reload ();
}//Init


//Reload
bool Profiles::Reload ()
{
    Err fErr (m_Err, L"Reload");
    if (m_sDirectory.empty ())
    {
        return fErr.Set (L"Profilesystem is not initialized");
    }

    std::vector<wstring> lsFiles;
    if (!ListFiles (m_sDirectory, &lsFiles))
    {
        return fErr.Set (L"Could not list the profiles");
    }

    m_lProfiles.clear ();
    for (unsigned int i = 0; i != lsFiles.size (); i++)
    {
        if (!AddProfile (lsFiles.at (i)))
        {
            return false;
        }
    }

    return true;
}//Reload


//GetList
bool Profiles::GetList (std::vector<Profile> **ppList)
{
    Err fErr (m_Err, L"GetList");
    if (ppList == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    *ppList = &m_lProfiles;

    return true;
}//GetList


//GetInput
bool Profiles::GetInput (enInput nInput, bool *pbState, bool bMouseMovement)
{
    *pbState = false;
    for (unsigned int i = 0; i != m_lProfiles.size (); i++)
    {
        if (!GetInput (nInput, pbState, i, bMouseMovement))
        {
            return false;
        }
        if (*pbState)
        {
            break;
        }
    }

    return true;
}//GetInput


//GetInput
bool Profiles::GetInput (enInput nInput, bool *pbState,
                         unsigned int nProfileId, bool bMouseMovement)
{
    Err fErr (m_Err, L"GetInput");
    if (pbState == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    if (!CheckProfileId (nProfileId))
    {
        return false;
    }

    *pbState = false;
    Key *pKey = NULL;

    switch (nInput)
    {
        case (eSpecial):
            pKey = &m_lProfiles.at (nProfileId).Special;
            break;
        case (eLeft):
            pKey = &m_lProfiles.at (nProfileId).Left;
            break;
        case (eRight):
            pKey = &m_lProfiles.at (nProfileId).Right;
            break;
            break;
    }

    if (pKey != NULL && pKey->IsPressed (bMouseMovement))
    {
        *pbState = true;
    }

    return true;
}//GetInput


//GetName
bool Profiles::GetName (unsigned int nProfileId, wstring *psName)
{
    Err fErr (m_Err, L"GetName");
    if (!CheckProfileId (nProfileId))
    {
        return false;
    }

    return FilterExtension (m_lProfiles.at (nProfileId).
                             ProfileConfig.GetFileName (),
                             psName);

    return true;
}//GetName


//GetByName
bool Profiles::GetByName (wstring sName, Config **ppProfile, bool *pbCreate)
{
    Err fErr (m_Err, L"GetByName");
    if (ppProfile == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    for (unsigned int i = 0; i != m_lProfiles.size (); i++)
    {
        wstring sCurName;
        if (!GetName (i, &sCurName))
        {
            return false;
        }
        if (sCurName == sName)
        {
            *ppProfile = &m_lProfiles.at (i).ProfileConfig;
            return true;
        }
    }

    return fErr.Set (L"A profile with the given name does not exists: " +
                     sName);
}//GetByName


//AddProfile
bool Profiles::AddProfile (wstring sFileName)
{
    Err fErr (m_Err, L"AddProfile");
    if (sFileName.empty ())
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    Profile NewProfile;
    wstring sLeft, sRight, sSpecial;
    sFileName = m_sDirectory + sFileName;
    if (!NewProfile.ProfileConfig.Init (&m_Err, sFileName, true) ||
        !NewProfile.ProfileConfig.Get (sKEY_LEFT, &sLeft) ||
        !NewProfile.ProfileConfig.Get (sKEY_RIGHT, &sRight) ||
        !NewProfile.ProfileConfig.Get (sKEY_SPECIAL, &sSpecial) ||
        !NewProfile.Left.Init (&m_Err, m_pWindow, sLeft) ||
        !NewProfile.Right.Init (&m_Err, m_pWindow, sRight) ||
        !NewProfile.Special.Init (&m_Err, m_pWindow, sSpecial))
    {
        return false;
    }
    m_lProfiles.push_back (NewProfile);

    return true;
}//AddProfile


//DeleteProfile
bool Profiles::DeleteProfile (wstring sName)
{
    Err fErr (m_Err, L"DeleteProfile");

    Config *pProfile;
    if (!GetByName (sName, &pProfile))
    {
        return false;
    }
    if (remove (ConStr<char> (m_sDirectory +
                              pProfile->GetFileName ()).c_str ()) != 0)
    {
        return fErr.Set (L"Could not remove the given profile file");
    }

    return Reload ();
}//DeleteProfile


//CheckProfileId
bool Profiles::CheckProfileId (unsigned int nProfileId)
{
    Err fErr (m_Err, L"CheckProfileId");
    if (nProfileId == m_lProfiles.size ())
    {
        return fErr.Set (L"A profile with the given id does not exist: " +
                         NumToStr (nProfileId));
    }

    return true;
}//CheckProfileId
