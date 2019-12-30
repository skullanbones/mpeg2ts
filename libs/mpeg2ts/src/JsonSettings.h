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
#ifndef _JSONSETTINGS_H
#define _JSONSETTINGS_H

/*!
 * This file loads the settings json file which contains
 * runtime configurations...
 */

#include <exception>   // for exception
#include <string>             // for string

// 3rd-party
#include "nlohmann/json.hpp"  // for json


using json = nlohmann::json;

class Settings
{
public:
    explicit Settings() = default;
    ~Settings() = default;

    bool loadFile(std::string);
    void loadJson(json);
    std::string getLogLevel() const;
    std::string getLogFileName() const;
    int getLogFileMaxSize() const;
    int getLogFileMaxNumberOf() const;

private:
    json mJ;
};

class LoadException : public std::exception
{
private:
    std::string m_message;

public:
    explicit LoadException() = default;
    explicit LoadException(const std::string& a_message);
    explicit LoadException(const std::exception a_e);
    virtual const char* what() const throw()
    {
        return m_message.c_str();
    }
};

#endif /* _JSONSETTINGS_H */
