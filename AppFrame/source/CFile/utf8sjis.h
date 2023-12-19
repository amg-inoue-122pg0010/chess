#pragma once

#ifdef WIN32
#include <Windows.h>
#endif
#include <string>

int ConvUTF8toSJIS(const unsigned char *pszUtf8, unsigned char *pszOutSjis);
std::string ConvUTF8toSJIS(std::string strUtf8);
int ConvSJIStoUTF8(const unsigned char *pszSjis, unsigned char *pszOutUtf8);
std::string ConvSJIStoUTF8(std::string strSjis);

