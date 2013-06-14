#include "Ball.hpp"

std::vector<Line> *Ball::s_plWalls     = NULL;
std::vector<Shield> *Ball::s_plShields = NULL;
std::vector<Goal> *Ball::s_plGoals     = NULL;
unsigned int       Ball::s_nRadius     =  0;
unsigned int       Ball::s_nSpeed      =  0;

//Init
bool Ball::Init (Err *pErr, sf::RenderWindow *pWindow, Vector2f vPos)
{
    if (!m_Err.SetClass (pErr, L"Ball") ||
        !Render::Init (pErr, pWindow))
    {
        return false;
    }
    Err fErr (m_Err, L"Ball");
    if (s_plWalls == NULL || s_plShields == NULL || s_plGoals == NULL)
    {
        return fErr.Set (L"They are no line lists");
    }

    m_vPos = vPos;
    m_vDir = Normalize (Vector2f (static_cast<float> (rand () % 200 - 100),
                                  static_cast<float> (rand () % 200 - 100)));
    //m_vDir = Normalize (Vector2f (1, 1));
    m_bStop = false;

    return true;
}//Init


//Render
bool Ball::Render ()
{
    sf::Shape Circle = sf::Shape::Circle (m_vPos, s_nRadius, sf::Color::White);
    Draw (Circle);

    return true;
}//Render


//Move
bool Ball::Move ()
{
    if (m_bStop)
    {
        return true;
    }

    float nLeftWay = m_Timer.GetElapsedTime () * s_nSpeed;
    m_Timer.Reset ();
    float nDistance = 0;
    Line *pnClosestLine = NULL;
    while (nLeftWay > 0)
    {
        if (!CheckLineCollisions (s_plGoals, nLeftWay,
                                  &nDistance, &pnClosestLine) ||
            !CheckLineCollisions (s_plWalls, nLeftWay,
                                  &nDistance, &pnClosestLine) ||
            !CheckLineCollisions (s_plShields, nLeftWay,
                                  &nDistance, &pnClosestLine))
        {
            return false;
        }

        if (nDistance == 0)
        {
            m_vPos.x += m_vDir.x * nLeftWay;
            m_vPos.y += m_vDir.y * nLeftWay;
            break;
        }
        m_vPos.x += m_vDir.x * nDistance;
        m_vPos.y += m_vDir.y * nDistance;
        m_bStop = true;
        break;
    }

    return true;
}//Move


//SetLines
void Ball::SetLines (std::vector<Line> *plWall,
                     std::vector<Shield> *plShields,
                     std::vector<Goal> *plGoals)
{
    s_plWalls = plWall;
    s_plShields = plShields;
    s_plGoals = plGoals;
}//SetLines


//SetRadius
void Ball::SetRadius (unsigned int nRadius)
{
    s_nRadius = nRadius;
}//SetRadius


//SetSpeed
void Ball::SetSpeed (unsigned int nSpeed)
{
    s_nSpeed = nSpeed;
}//SetSpeed


//CheckLineCollision
bool Ball::CheckLineCollision (bool *pbCollision, Vector2f *pvIntercept,
                               Vector2f vFirstBegin, Vector2f vFirstEnd,
                               Vector2f vSecondBegin, Vector2f vSecondEnd)
{
    Err fErr (m_Err, L"CheckLineCollision");
    if (pbCollision == NULL || pvIntercept == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }

    float nFirstM, nFirstB, nSecondM, nSecondB;
    if (!CalcFuncFactors (vFirstBegin, vFirstEnd, &nFirstM, &nFirstB) ||
        !CalcFuncFactors (vSecondBegin, vSecondEnd, &nSecondM, &nSecondB))
    {
        return false;
    }
    Vector2f vIntercept (-(nFirstB - nSecondB) / (nFirstM - nSecondM), 0);
    if (vSecondBegin.x == vSecondEnd.x)
    {
        vIntercept.x = vSecondBegin.x;
    }
    vIntercept.y = (nFirstM * vIntercept.x) + nFirstB;
    if (vFirstBegin.x == vFirstEnd.x)
    {
        vIntercept.x = vFirstBegin.x;
        vIntercept.y = (nSecondM * vIntercept.x) + nSecondB;
    }

    if (abs (vFirstBegin.x - vIntercept.x) <= abs (vFirstBegin.x - vFirstEnd.x) &&
        abs (vFirstBegin.y - vIntercept.y) <= abs (vFirstBegin.y - vFirstEnd.y) &&
        abs (vSecondBegin.x - vIntercept.x) <= abs (vSecondBegin.x - vSecondEnd.x) &&
        abs (vSecondBegin.y - vIntercept.y) <= abs (vSecondBegin.y - vSecondEnd.y) &&
        abs (vSecondEnd.x - vIntercept.x) <= abs (vSecondEnd.x - vSecondBegin.x) &&
        abs (vSecondEnd.y - vIntercept.y) <= abs (vSecondEnd.y - vSecondBegin.y))
    {
        *pbCollision = true;
        *pvIntercept = vIntercept;
    }

    return true;
}//CheckLineCollision
