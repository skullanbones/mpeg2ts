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
