#include "JsonSettings.h"

// Dont allow json.hpp throw exceptions
#define JSON_THROW_USER

#include "json.hpp"

#include <fstream>
#include <iostream>
#include <string>

using json = nlohmann::json;


bool Settings::loadFile(std::string file)
{
    // 1. Open file
    std::ifstream ifs(file);
    if (ifs.fail())
    {
        std::string errMsg = "Could not find settings file: " + file;
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
        std::cerr << "Could not load asset file: " << file << ", with exception: " << e.what() << std::endl;
        std::string errMsg = "Could not load asset file: " + file + ", with exception: " + e.what();
        throw LoadException(errMsg);
        return false;
    }
}

void Settings::loadJson(json js)
{
    mJ = js;
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

LoadException::LoadException(const std::string& message)
: message_(message)
{
}

LoadException::LoadException(const std::exception e)
: message_(e.what())
{
}
