//Manages all messages in the game

#pragma once

#include <list>

#include "Err.hpp"
#include "Render.hpp"

class Message : Render
{
    public:

    //Initializes
    bool Init (Err *pErr, sf::RenderWindow *pWindow);
    //Renders the lines
    bool Render ();
    //Add a line
    bool Add (wstring sText);
    //Clear the line list
    void Clear ();

    private:

    struct Line
    {
        wstring sText;
        float nPos;
    };

    Err               m_Err;
    std::list<Line>   m_lLines;
    float             m_LastFrame;
    sf::Clock         m_Timer;
};
