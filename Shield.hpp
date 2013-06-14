//Manages a single shield

#pragma once

#include <cmath>

#include "Goal.hpp"
#include "Line.hpp"
#include "Message.hpp"
#include "Profiles.hpp"
#include "Render.hpp"

class Shield : public Line
{
    public:

    Shield ();
    //Initializes and sets the range of motion
    bool Init (Err *pErr, sf::RenderWindow *pWindow, Profile *pProfile,
               unsigned int nStartGoal);
    //Manages the user inputs and moves the shield
    bool Move ();

    //Sets the goal list
    static void SetGoalList (std::vector<Goal> *plGoals);
    //Sets the shield speed
    static void SetSpeed (unsigned int nSpeed);
    //Sets the pointer of the message manager
    static void SetMessage (Message *pMessage);
    //Sets the pointer of the language manager
    static bool SetLanguage (Config *pLanguage);

    private:

    //Moves shield to the current position
    void SetCoordinates ();
    //Calculates the current range of motion
    void CalcRange ();
    //Switches the current goal
    void SwitchGoal (bool bFurther);
    //Sets the shield on the given end of the goal
    void MoveToEnd (bool bBegin);
    //Fixates the current goal
    void Fixate ();
    //Sets the direction of movement
    void SetDirection (bool bFurther);

    enum enState
    {
        ePlay = eEnd,
        eDead //A user played the game und losed
    };

    enum enMovement
    {
        eStop,
        eFurther,
        eBack
    };

    Err          m_Err;
    Profile     *m_pProfile;
    sf::Clock    m_Timer;
    float        m_nSpecialPressTime;
    float        m_nPosition;
    float        m_nMaxRange;
    int          m_nCurGoal;
    unsigned int m_nMovement;

    static std::vector<Goal> *s_plGoals;
    static unsigned int       s_nSpeed;
    static Message           *s_pMessage;
    static wstring            s_sJoined;

    const static float s_nSize;
};
