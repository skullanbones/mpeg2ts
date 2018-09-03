#pragma once

/*!
 * This file loads the settings json file which contains
 * runtime configurations...
 */
#include <string>

/// 3rd-party
#include "json.hpp"

using json = nlohmann::json;

class Settings
{
public:
    explicit Settings() = default;
    ~Settings() = default;

    void loadFile(std::string);
    void loadJson(json js);
    std::string getLogLevel() const;
    std::string getLogFileName() const;
    int getLogFileMaxSize() const;
    int getLogFileMaxNumberOf() const;
    
private:
    json mJ;
};
