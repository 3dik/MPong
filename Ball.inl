//Look at "Ball.hpp"

//CheckLineCollisions

template <class tLine>
bool Ball::CheckLineCollisions (std::vector<tLine> *plLines,
                                float nLeftWay, float *pnDistance,
                                Line **ppClosestLine)
{
    Err fErr (m_Err, L"CheckLineCollisions");
    if (plLines == NULL || pnDistance == NULL || ppClosestLine == NULL)
    {
        return fErr.Set (sERR_ARGUMENTS);
    }
    if (plLines->empty ())
    {
        return true;
    }

    Vector2f vNewPos (m_vPos.x + (m_vDir.x * nLeftWay),
                      m_vPos.y + (m_vDir.y * nLeftWay));
    for (unsigned int i = 0; i != plLines->size (); i++)
    {
        bool bIntercept = false;
        Vector2f vIntercept;
        if (!CheckLineCollision (&bIntercept, &vIntercept,
                                 m_vPos, vNewPos,
                                 plLines->at (i).GetBegin (),
                                 plLines->at (i).GetEnd ()))
        {
            return false;
        }
        if (bIntercept)
        {
            float nDistance = Distance (vIntercept - m_vPos);
            if (nDistance > *pnDistance && *pnDistance != 0)
            {
                continue;
            }
            *pnDistance = nDistance;
            *ppClosestLine = &plLines->at (i);
        }
    }

    return true;
}//CheckLineCollisions
