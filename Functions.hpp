//Miscellaneous functions

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "dirent.h"
#include "sys/stat.h"
#include "errno.h"
#include "SFML/Graphics.hpp"

#include "Convert.hpp"
#include "Misc.hpp"

using std::wstring;
using sf::Vector2f;

//Output a string
template <class tOutput>
void OPut (tOutput tOut);

//Returns a list containing the files of the given directory
//with the game file extension
bool ListFiles (wstring sPath, std::vector<wstring> *plsFiles);

//Returns the file name without the file extension. If the last error is a NULL
//pointer, the function checks only for a valid game file
bool FilterExtension (wstring sFileName, wstring *psValue = NULL);

//Normalizes the given vector
template <class tvValue>
tvValue Normalize (tvValue vValue);

//Calculates the distance of the given vector
template <class tnNum>
tnNum Distance (sf::Vector2<tnNum> vValue);

//Calculates the function factors for a linear function
bool CalcFuncFactors (Vector2f vBegin, Vector2f vEnd,
                      float *pnM, float *pnB, bool *pbForward = NULL);

#include "Functions.inl"
