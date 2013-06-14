//Entry point

#include "iostream"

#include "Game.hpp"
#include "Err.hpp"
#include "Misc.hpp"

//main
int main ()
{
    Err Error;
    Game MPong;
    if (!MPong.Init (&Error) || !MPong.Run ())
    {
        return nEXIT_FAIL;
    }
    return nEXIT_OK;
}//main
