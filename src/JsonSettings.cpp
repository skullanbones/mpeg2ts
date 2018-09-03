#include "JsonSettings.h"

#include "json.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>

using json = nlohmann::json;

bool Settings::loadFile(std::string file)
{
    try
    {
        std::ifstream ifs(file);
        ifs >> mJ;
        return true;
    }
    catch (std::exception& e)
    {
        std::cerr << "Could not load asset file: " << file << ", with exception: " << e.what() << std::endl;
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
};

int Settings::getLogFileMaxSize() const
{
    return mJ["settings"]["logFileMaxSize"];
};

int Settings::getLogFileMaxNumberOf() const
{
    return mJ["settings"]["logFileMaxNumberOf"];
};
