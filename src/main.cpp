/**
 * This file is part of QFM.
 *
 * Copyright (C) 2011-2015 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * QFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QFM. If not, see <http://www.gnu.org/licenses/>.
 */

#include "application.h"

#include <cstring>


int main(int argc, char **argv)
{
    static const char file_name[] = "/.qfm/settings.xml";

    char settings[128];
    settings[0] = 0;

    if (char *home = ::getenv("HOME"))
    {
        size_t len = ::strlen(home);

        if (len < sizeof(settings) && sizeof(file_name) <= sizeof(settings) - len)
        {
            ::memcpy(settings, home, len);
            ::memcpy(settings + len, file_name, sizeof(file_name));
        }
        else
            ::memcpy(settings, file_name + 1, sizeof(file_name) - 1);
    }
    else
        ::memcpy(settings, file_name + 1, sizeof(file_name) - 1);


    return Application(settings, argc, argv).exec();
}
