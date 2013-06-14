//Wrapper class for RenderWindow
//Converts the relative position and size data to appropriate pixel data

#pragma once

#include "SFML/Graphics.hpp"

#include "Err.hpp"
#include "Misc.hpp"

using sf::Vector2f;

class Render
{
    public:

    //Sets the current resolution (not the window size!)
    static void SetRes (Vector2f vRes);
    //Returns the current resolution (not the window size!)
    static Vector2f &GetRes ();

    protected:

    Render ();
    //Initializes, every class should call this function
    bool Init (Err *pErr, sf::RenderWindow *pWindow);
    //Wrapper function for "Draw" of "RenderWindow", converts the relative
    //position and size data to appropriate pixel data for rendering
    template <class tObject>
    void Draw (const tObject &rObject) const;

    //Converts given relative data to appropriate pixel data for rendering
    template <class tvXY>
    sf::Vector2<tvXY> Calc (sf::Vector2<tvXY> vVal) const;
    template <class tnX>
    tnX CalcX (tnX nX) const;
    template <class tnY>
    tnY CalcY (tnY nY) const;

    //Converts given resolution pixel data appropriate relativ data
    template <class tvXY>
    sf::Vector2<tvXY> Rel (sf::Vector2<tvXY> vVal) const;
    template <class tnX>
    tnX RelX (tnX nX) const;
    template <class tnY>
    tnY RelY (tnY nY) const;

    sf::RenderWindow *m_pWindow;

    private:

    Err m_Err;

    static Vector2f s_vRes;
};

#include "Render.inl"
