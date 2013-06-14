//Main class of the game

#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"

#include "Config.hpp"
#include "Err.hpp"
#include "Match.hpp"
#include "Menu.hpp"
#include "Profiles.hpp"

class Game
{
    public:

    Game  ();
    ~Game ();
    bool Init (Err *pErr);
    bool Run ();

    private:

    Config   m_Config;
    Config   m_Language;
    Err      m_Err;
    Menu     m_Menu;
    Profiles m_Profiles;

    sf::RenderWindow m_Window;
};
