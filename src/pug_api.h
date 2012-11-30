#ifndef PUGAPI_H
#define PUGAPI_H

#include <string>
#include <vector>
#include <map>

#include "pugcoreapplication.h"

class PugAPI {
public:
    PUGApi(const std::vector<std::string> argv);

    const std::map<std::string, std::string> parse(const std::string path) const;
    const std::string map(const std::map<std::string, std::string> dict) const;

private:
    PugCoreApplication m_app;
};

#endif
