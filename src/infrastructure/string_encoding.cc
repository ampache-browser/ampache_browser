// string_encoding.cc
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2023 Róbert Čerňanský




#ifdef _WIN32

#include <string>
#include <windows.h>
#include "infrastructure/string_encoding.h"

using namespace std;



namespace infrastructure {

    string StringEncoding::wideToUtf8(const std::wstring& wideStr) {
        if (wideStr.empty()) {
            return std::string();
        }
        int requiredSize = WideCharToMultiByte(CP_UTF8, 0, &wideStr[0], (int)wideStr.length(), NULL, 0, NULL, NULL);
        std::string resultStr(requiredSize, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wideStr[0], (int)wideStr.length(), &resultStr[0], requiredSize, NULL, NULL);
        return resultStr;
    }

    wstring StringEncoding::utf8ToWide(const std::string& utf8Str) {
        if (utf8Str.empty()) {
            return std::wstring();
        }
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.length(), NULL, 0);
        std::wstring resultStr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &utf8Str[0], (int)utf8Str.length(), &resultStr[0], size_needed);
        return resultStr;
    };

}

#endif // _WIN32
