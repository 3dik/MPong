//Look at "Functions.hpp"

//OPut
template <class tOutput>
void OPut (tOutput tOut)
{
    std::wcout<<tOut<<std::endl;
}//OPut


//Normalize
template <class tvValue>
tvValue Normalize (tvValue vValue)
{
    float nLen = Distance (vValue);
    return tvValue (vValue.x / nLen, vValue.y / nLen);
}//Normalize


//Distance
template <class tnNum>
tnNum Distance (sf::Vector2<tnNum> vValue)
{
    return sqrt (pow (vValue.x, 2) + pow (vValue.y, 2));
}//Distance
