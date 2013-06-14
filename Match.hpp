//Manages the matches

#pragma once

#include "SFML/Graphics.hpp"

#include "Ball.hpp"
#include "Config.hpp"
#include "Err.hpp"
#include "Goal.hpp"
#include "Line.hpp"
#include "Message.hpp"
#include "Misc.hpp"
#include "Profiles.hpp"
#include "Render.hpp"
#include "Shield.hpp"

class Match : public Render
{
    public:

    Match ();
    ~Match ();
    //Initializes
    bool Init (Err *pErr, Config *pConfig, Config *pLanguage,
               Profiles *pProfiles, sf::RenderWindow *pWindow,
               int nFirstBalls);
    //Starts the first match and runs until the user quits
    bool Run (bool *pbQuit);

    private:

    //Loads the next match
    bool Reinit ();
    //Loads the level data
    bool LoadLevel ();
    //Adds a wall to the list
    bool AddWall (Vector2f vBegin, Vector2f vEnd, bool bVisible);
    //Loads the shields
    bool LoadShields ();
    //Renders
    bool Render ();
    //Moves the game
    bool Move ();
    //Renders a line list
    template <class tLine>
    bool RenderLineList (const std::vector<tLine> &lLines);
    //Converts the given level vector to a sfml vector
    bool ConvertVector (wstring svPos, Vector2f *pvValue);

    Err                 m_Err;
    Config             *m_pConfig;
    Config             *m_pLanguage;
    Profiles           *m_pProfiles;
    Message             m_Message;
    bool                m_bClose;
    int                 m_nFirstBalls;
    std::vector<Line>   m_lWalls;
    std::vector<Goal>   m_lGoals;
    std::vector<Shield> m_lShields;
    std::list<Ball>     m_lBalls;
};

#include "Match.inl"
