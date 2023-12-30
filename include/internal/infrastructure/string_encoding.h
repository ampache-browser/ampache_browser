// string_encoding.h
//
// Project: Ampache Browser
// License: GNU GPLv3
//
// Copyright (C) 2015 - 2023 Róbert Čerňanský



#ifdef _WIN32

#ifndef STRINGENCODING_H
#define STRINGENCODING_H



#include <string>



namespace infrastructure {

    /**
     * @brief Converts between string encodings.
     */
    class StringEncoding {

    public:
        /**
         * @brief Converts from multibyte to UTF8.
         */
        static std::string wideToUtf8(const std::wstring& wideStr);

        /**
         * @brief Converts from UTF8 to multibyte.
         */
        static std::wstring utf8ToWide(const std::string& utf8Str);
    };

}

#endif // STRINGENCODING_H

#endif // _WIN32
