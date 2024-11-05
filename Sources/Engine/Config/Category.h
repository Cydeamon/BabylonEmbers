#pragma once

#include <map>
#include <string>
#include <vector>

struct Category
{
    std::string name;
    std::map<std::string, std::string> values;
};