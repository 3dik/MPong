//Manages a single ball

#pragma once

#include "Goal.hpp"
#include "Line.hpp"
#include "Render.hpp"
#include "Shield.hpp"

class Ball : public Render
{
    public:

    //Initializes
    bool Init (Err *pErr, sf::RenderWindow *pWindow, Vector2f vPos);
    //Renders
    bool Render ();
    //Moves the ball
    bool Move ();

    //Sets the lists for the lines
    static void SetLines (std::vector<Line> *plWall,
                          std::vector<Shield> *plShields,
                          std::vector<Goal> *plGoals);
    //Sets speed
    static void SetSpeed (unsigned int nSpeed);
    //Sets radius
    static void SetRadius (unsigned int nRadius);

    private:

    //Checks for collisions of the ball with the given line list
    //If the function founds a closer line, it updates the cur distance and line
    template <class tLine>
    bool CheckLineCollisions (std::vector<tLine> *plLines,
                              float nLeftWay, float *pnDistance,
                              Line **ppClosestLine);
    //Checks for collision betweet two lines
    bool CheckLineCollision (bool *pbCollision, Vector2f *pvIntercept,
                             Vector2f vFirstBegin, Vector2f vFirstEnd,
                             Vector2f vSecondBegin, Vector2f vSecondEnd);

    Err       m_Err;
    Vector2f  m_vPos;
    Vector2f  m_vDir;
    sf::Clock m_Timer;
    bool      m_bStop;
    float     m_nM;
    float     m_nB;
    float     m_nX;
    bool      m_bForward;

    static std::vector<Line>   *s_plWalls;
    static std::vector<Shield> *s_plShields;
    static std::vector<Goal>   *s_plGoals;
    static unsigned int         s_nRadius;
    static unsigned int         s_nSpeed;
};

#include "Ball.inl"
