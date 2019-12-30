/*****************************************************************
*
*    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
*
*    mpeg2ts - mpeg2ts lib
*
*    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
*
*    Unless you have obtained mpeg2ts under a different license,
*    this version of mpeg2ts is mpeg2ts|GPL.
*    Mpeg2ts|GPL is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License as
*    published by the Free Software Foundation; either version 2,
*    or (at your option) any later version.
*
*    Mpeg2ts|GPL is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with mpeg2ts|GPL; see the file COPYING.  If not, write to the
*    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
*    02111-1307, USA.
*
********************************************************************/
#include "JsonSettings.h"

// Dont allow json.hpp throw exceptions
#define JSON_THROW_USER

#include "nlohmann/json.hpp"  // for basic_json<>::value_type, basic_json

#include <fstream>
#include <iostream>           // for operator<<, basic_ostream, ifstream, cerr
#include <string>             // for allocator, operator+, string, char_traits

using json = nlohmann::json;


bool Settings::loadFile(std::string a_file)
{
    // 1. Open file
    std::ifstream ifs(a_file);
    if (ifs.fail())
    {
        std::string errMsg = "Could not find settings file: " + a_file;
        throw LoadException(errMsg);
        return false;
    }

    // 2. Load and read file
    try
    {
        ifs >> mJ;
        return true;
    }
    catch (std::exception& e)
    {
        std::cerr << "Could not load asset file: " << a_file << ", with exception: " << e.what() << '\n';
        std::string errMsg = "Could not load asset file: " + a_file + ", with exception: " + e.what();
        throw LoadException(errMsg);
        return false;
    }
}

void Settings::loadJson(json a_js)
{
    mJ = a_js;
}

std::string Settings::getLogLevel() const
{
    std::string str = mJ["settings"]["logLevel"];
    return str;
}

std::string Settings::getLogFileName() const
{
    std::string str = mJ["settings"]["logFileName"];
    return str;
}

int Settings::getLogFileMaxSize() const
{
    return mJ["settings"]["logFileMaxSize"];
}

int Settings::getLogFileMaxNumberOf() const
{
    return mJ["settings"]["logFileMaxNumberOf"];
}

///////////////// LoadException /////////////////////////

LoadException::LoadException(const std::string& a_message)
    : m_message(a_message)
{
}

LoadException::LoadException(const std::exception a_e)
    : m_message(a_e.what())
{
}
