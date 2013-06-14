//Look at Render.hppganjaganja


//Draw
template <class tObject>
void Render::Draw (const tObject &rObject) const
{
    tObject Object = rObject;
    Object.SetPosition (Calc (rObject.GetPosition ()));
    Object.SetScale (Calc (rObject.GetScale ()));
    Object.SetCenter (rObject.GetCenter ().x / rObject.GetScale ().x,
                      rObject.GetCenter ().y / rObject.GetScale ().y);

    m_pWindow->Draw (Object);
}//Draw

//Calc
template <class tvXY>
sf::Vector2<tvXY> Render::Calc (sf::Vector2<tvXY> vVal) const
{
    return sf::Vector2<tvXY> (CalcX (vVal.x), CalcY (vVal.y));
}//Calc


//CalcX
template <class tnX>
tnX Render::CalcX (tnX nX) const
{
    //I used own resolution data because "GetWidth" does not changes if the
    //game is in window mode and the user resized the window.
    //But instead of changing the resolution, the pixel are streched only. So
    //the really resolution is constant.
    return s_vRes.x / 1000 * nX;
}//CalcX


//CalcY
template <class tnY>
tnY Render::CalcY (tnY nY) const
{
    return s_vRes.y / 750 * nY;
}//CalcY



//Rel
template <class tvXY>
sf::Vector2<tvXY> Render::Rel (sf::Vector2<tvXY> vVal) const
{
    return sf::Vector2<tvXY> (RelX (vVal.x), RelY (vVal.y));
}//Rel


//RelX
template <class tnX>
tnX Render::RelX (tnX nX) const
{
    //Here I used GetWidth because the position data, wich are from Sfml,
    //are absolute.
    return nX / m_pWindow->GetWidth () * 1000;
}//RelcX


//RelY
template <class tnY>
tnY Render::RelY (tnY nY) const
{
    return nY / m_pWindow->GetHeight () * 750;
}//RelY
