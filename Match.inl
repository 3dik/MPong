//Look at Match.hpp

//RenderLineList
template <class tLine>
bool Match::RenderLineList (const std::vector<tLine> &lLines)
{
    for (unsigned int i = 0; i != lLines.size (); i++)
    {
        if (!lLines.at (i).Render ())
        {
            return false;
        }
    }
    return true;
}//RenderLineList
