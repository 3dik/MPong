//Manages a single line

#pragma once

#include "SFML/Graphics.hpp"

#include "Err.hpp"
#include "Render.hpp"

using sf::Vector2f;

class Line : public Render
{
    public:

    Line ();
    //Initializes
    bool Init (Err *pErr, sf::RenderWindow *pWindow,
               Vector2f vBegin = Vector2f (0, 0),
               Vector2f vEnd = Vector2f (0, 0),
               bool bVisible = true);
    //Renders
    virtual bool Render () const;
    //Returns the begin point of the line
    Vector2f GetBegin () const;
    //Returns the end point
    Vector2f GetEnd () const;

    protected:

    void DrawLine (sf::Color Color) const;

    enum enState
    {
        eInactive,
        eActive,
        eEnd      //Only for other enums
    };

    Vector2f     m_vBegin;
    Vector2f     m_vEnd;
    unsigned int m_nState;

    private:

    Err m_Err;
    bool m_bVisible;
};
