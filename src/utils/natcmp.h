/*
  natcmp.h -- Perform 'natural order' comparisons of strings in C++.
  This is a modified version of Martin Pool's strnatcmp, more suited for C++
  rather than C, also distrubuted as a header only library.

  https://github.com/sourcefrog/natsort

  Copyright (C) 2000, 2004 by Martin Pool <mbp sourcefrog net>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef NATCMP_H
#define NATCMP_H

#ifdef _WIN32
#include "utils/utf8conv_win32.h"
#include <shlwapi.h>
#else
#include <cctype>
#endif
#include <string>

namespace utils {
#ifndef _WIN32
inline int to_upper(const char ch) {
    return std::toupper(static_cast<unsigned char>(ch));
}
inline int is_digit(const char ch) {
    return std::isdigit(static_cast<unsigned char>(ch));
}
inline int is_ws(const char ch) {
    return std::isspace(static_cast<unsigned char>(ch));
}
inline int natcmp(const std::string &ls, const std::string &rs) {

    for (std::string::size_type li = 0, ri = 0;; ++li, ++ri) {

        // Skip whitespace
        while (is_ws(ls[li]) != 0) {
            ++li;
        }
        while (is_ws(rs[ri]) != 0) {
            ++ri;
        }

        // Check to see if either is a digit
        if ((is_digit(ls[li]) != 0) && (is_digit(rs[ri]) != 0)) {
            if (ls[li] == '0' || rs[ri] == '0') {
                // Compare two left-aligned numbers: the first to have a
                // different value wins.
                for (;; li++, ri++) {
                    const int l_isdig = is_digit(ls[li]);
                    const int r_isdig = is_digit(rs[ri]);
                    if ((l_isdig == 0) && (r_isdig == 0)) {
                        break;
                    }
                    if (l_isdig == 0) {
                        return -1;
                    }
                    if (r_isdig == 0) {
                        return 1;
                    }
                    if (ls[li] < rs[ri]) {
                        return -1;
                    }
                    if (ls[li] > rs[ri]) {
                        return 1;
                    }
                }
            } else {
                // The longest run of digits wins. That aside, the greatest
                // value wins, but we can't know that it will until we've
                // scanned both numbers to know that they have the same
                // magnitude, so we remember it in BIAS.
                int bias = 0;
                for (;; li++, ri++) {
                    const int l_isdig = is_digit(ls[li]);
                    const int r_isdig = is_digit(rs[ri]);
                    if ((l_isdig == 0) && (r_isdig == 0)) {
                        break;
                    }
                    if (l_isdig == 0) {
                        return -1;
                    }
                    if (r_isdig == 0) {
                        return 1;
                    }
                    if (ls[li] < rs[ri]) {
                        if (bias == 0) {
                            bias = -1;
                        }
                    } else if (ls[li] > rs[ri]) {
                        if (bias == 0) {
                            bias = 1;
                        }
                    } else if ((ls[li] == 0) && (rs[ri] == 0)) {
                        break;
                    }
                }
                if (bias != 0) {
                    return bias;
                }
            }
        }

        // At the end of both strings - they're the same
        if ((ls[li] == 0) && (rs[ri] == 0)) {
            return 0;
        }

        // Always fold case before comparing
        const int lc = to_upper(ls[li]);
        const int rc = to_upper(rs[ri]);
        if (lc < rc) {
            return -1;
        }
        if (lc > rc) {
            return 1;
        }

        // Same characters, iterate
    }
}
#else
inline int natcmp(const std::string &ls, const std::string &rs) {
    return StrCmpLogicalW(&widen(ls)[0], &widen(rs)[0]);
}
inline int natcmp(const std::wstring &ls, const std::wstring &rs) {
    return StrCmpLogicalW(&ls[0], &rs[0]);
}
#endif
} // namespace utils

#endif
