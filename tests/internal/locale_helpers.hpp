// Copyright (c) 2024-2026 xlnt-community
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE
//
// @license: http://www.opensource.org/licenses/mit-license.php
// @author: see AUTHORS file

#pragma once

#include <helpers/assertions.hpp>
#include <clocale>
#include <cstring>
#include <locale>

namespace test_helpers
{

struct SetLocale
{
    SetLocale(const char* locale_name, const char *expected_decimal_separator)
        : previous_locale(setlocale(LC_ALL, nullptr))
    {
        xlnt_assert(std::setlocale(LC_ALL, locale_name) != nullptr);

        if (strcmp(expected_decimal_separator, localeconv()->decimal_point) != 0)
        {
            std::string error = "Unexpected decimal separator for locale ";
            error += locale_name;
            error += " expected ";
            error += expected_decimal_separator;
            error += " but found ";
            error += localeconv()->decimal_point;

            // If failed, please install the locale specified by the CMake variable XLNT_LOCALE_****_DECIMAL_SEPARATOR
            // to correctly run this test *and* make sure that the locale uses the expected decimal separator,
            // or alternatively disable the CMake option XLNT_USE_LOCALE_****_DECIMAL_SEPARATOR.
            throw xlnt::invalid_parameter(error);
        }

    }
    ~SetLocale() {std::setlocale(LC_ALL, previous_locale);}

    char * previous_locale = nullptr;
};

// Unlike SetLocale (which only changes the C locale seen by e.g. setlocale/localeconv),
// this changes the *global C++ locale* (std::locale::global), which is what a default-constructed
// std::ostringstream is imbued with. This is required to reproduce bugs caused by code (such as
// libstudxml's default_value_traits<T>::serialize) that formats numbers through such a stream.
struct SetGlobalLocale
{
    SetGlobalLocale(const char *locale_name, char expected_thousands_separator)
        : previous_locale(std::locale::global(std::locale(locale_name)))
    {
        const auto &grouping = std::use_facet<std::numpunct<char>>(std::locale()).grouping();
        const auto actual_thousands_separator = std::use_facet<std::numpunct<char>>(std::locale()).thousands_sep();

        if (grouping.empty() || actual_thousands_separator != expected_thousands_separator)
        {
            std::locale::global(previous_locale);

            std::string error = "Unexpected digit grouping for locale ";
            error += locale_name;
            error += ": expected thousands separator '";
            error += expected_thousands_separator;
            error += "' with digit grouping enabled, but found '";
            error += actual_thousands_separator;
            error += "' with grouping ";
            error += grouping.empty() ? "disabled" : "enabled";

            // If failed, please install the locale specified by the CMake variable XLNT_LOCALE_****_DECIMAL_SEPARATOR
            // to correctly run this test *and* make sure that the locale groups digits using the expected
            // thousands separator, or alternatively disable the CMake option XLNT_USE_LOCALE_****_DECIMAL_SEPARATOR.
            throw xlnt::invalid_parameter(error);
        }
    }
    ~SetGlobalLocale() { std::locale::global(previous_locale); }

    std::locale previous_locale;
};

} // namespace test_helpers
