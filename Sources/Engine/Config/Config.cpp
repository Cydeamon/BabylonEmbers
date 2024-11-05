#include "Config.h"
#include <fstream>

Config &Config::GetInstance()
{
    static Config instance;
    return instance;
}


Category *Config::findCategory(std::string category)
{
    for (Category *cat: categories)
    {
        if (cat->name == category)
            return cat;
    }

    return nullptr;
}

void Config::Load()
{
    std::ifstream file(iniFileName);

    if (file.is_open())
    {
        std::string line;
        Category *category = nullptr;

        while (std::getline(file, line))
        {
            line = trimString(line);

            // Skip comments
            if (line[0] == '#')
                continue;

            // Skip empty lines
            if (line.empty())
                continue;

            // Category
            if (line[0] == '[' && line[line.size() - 1] == ']')
            {
                category = new Category();
                category->name = trimString(line.substr(1, line.size() - 2));

                if (category->name.empty())
                    throw std::runtime_error("Error while reading " + iniFileName + ".\nCategory name cannot be empty!");

                categories.push_back(category);
                continue;
            }

            // Value
            if (line.find('=') != std::string::npos)
            {
                if (!category)
                    throw std::runtime_error("Error while reading " + iniFileName + ".\nValues before categories are not allowed!");

                std::string name = trimString(line.substr(0, line.find_first_of('=')));
                std::string value = trimString(line.substr(line.find_first_of('=') + 1));
                category->values[name] = value;
            }
        }
    }
}

std::string Config::trimString(std::string string)
{
    string.erase(0, string.find_first_not_of(" \t"));
    string.erase(string.find_last_not_of(" \t") + 1);

    return string;
}

void Config::Save()
{
    std::ofstream file(iniFileName, std::ios::out | std::ios::trunc);

    if (!file.is_open())
        throw std::runtime_error("Failed to open " + iniFileName + " for writing!Check permissions.");

    if (file.is_open())
    {
        for (Category *category: categories)
        {
            file << "[" << category->name << "]" << std::endl;

            for (auto &value: category->values)
                file << value.first << " = " << value.second << std::endl;

            file << std::endl;
        }

        file.close();
    }
}