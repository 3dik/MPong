//Miscellaneous settings

#pragma once

#include <string>

using std::wstring;

//Return values of the entry point
const int nEXIT_OK   = 0;
const int nEXIT_FAIL = 1;

const double nPI = 3.1415926;
const float  nSLEEPTIME = 0.01;

const wstring sENDL = L"\n";

const wstring sFILEEXTENSION  = L".cfg";

const wstring sTITLE_SHORT    = L"MPon";
const wstring sTITLE_END      = L"g";
const wstring sTITLE          = sTITLE_SHORT + sTITLE_END;

const wstring sERR_LATER      = L"This error should not exists";
const wstring sERR_ARGUMENTS  = L"Invalid function arguments";
const wstring sERR_NOTINIT    = L"Object isnt initialized";
const wstring sERR_FILEFAIL   = L"Couldnt open file: ";

const wstring sCONF_LEVELDIR   = L"LevelDir";
const wstring sCONF_LEVELFILE  = L"LevelFile";
const wstring sCONF_FULLSCREEN = L"Fullscreen";
const wstring sCONF_HELP       = L"Help";

const wstring sKEY_SPECIAL    = L"Special";
const wstring sKEY_LEFT       = L"Left";
const wstring sKEY_RIGHT      = L"Right";

const wstring sMOUSE_UP = L"MouseMoveUp";
const wstring sMOUSE_DOWN = L"MouseMoveDown";
const wstring sMOUSE_LEFT = L"MouseMoveLeft";
const wstring sMOUSE_RIGHT = L"MouseMoveRight";

const wstring sLINE_FURTHER = L"Further";
const wstring sLINE_BACK    = L"Back";
const wstring sLINE_WALL    = L"Wall";
