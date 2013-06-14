//Manages a goal line

#pragma once

#include "Line.hpp"

class Goal : public Line
{
    public:

    Goal ();
    //Initializes
    bool Init (Err *pErr, sf::RenderWindow *pWindow, bool bFurther,
               Vector2f vBegin, Vector2f vEnd);
    //Renders
    bool Render () const;
    //Increases or deacreases the shield count
    void AdjustShieldCount (bool bIncrease);
    //Returns the directional of motion
    bool GetDirection ();

    //Increases or decreases the free shield count
    static void AdjustFreeShieldCount (bool bIncrease);

    private:

    Err  m_Err;
    int  m_nNumShields;
    bool m_bFurther;

    static int s_nNumFixedGoals;
    static int s_nNumFreeShields;
};
