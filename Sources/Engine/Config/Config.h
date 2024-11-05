#pragma once

#include <Config.h>
#include "Category.h"
#include <concepts>
#include <exception>
#include <stdexcept>

class Config
{
public:
    static Config &GetInstance();

    void Load();
    void Save();

    void SetIniFileName(std::string name) { iniFileName = name; }
    void SetSaveOnValuesChange(bool value) { saveOnValuesChange = value; }

    template <typename T>
    requires std::same_as<T, std::string> || std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, double> || std::same_as<T, bool>
    T GetValue(std::string categoryName, std::string name)
    {
        Category *category = findCategory(categoryName);

        if (!category)
        {
            char buffer[128];
            sprintf(buffer, "Category \"%s\" not found", categoryName.c_str());
            throw std::runtime_error(buffer);
        }

        auto it = category->values.find(name);

        if (it == category->values.end())
        {
            char buffer[128];
            sprintf(buffer, "Value \"%s\" not found in category \"%s\"", name.c_str(), categoryName.c_str());
            throw std::runtime_error(buffer);
        }

        if constexpr (std::is_same_v<T, int>)
            return std::stoi(it->second);
        else if constexpr (std::is_same_v<T, float>)
            return std::stof(it->second);
        else if constexpr (std::is_same_v<T, double>)
            return std::stod(it->second);
        else if constexpr (std::is_same_v<T, std::string>)
            return it->second;
        else if constexpr (std::is_same_v<T, bool>)
            return it->second == "1";
    }

    template <typename T>
    requires std::same_as<T, std::string> || std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, double> || std::same_as<T, bool>
    void SetValue(std::string categoryName, std::string name, T value)
    {
        Category *category = findCategory(categoryName);

        if (!category)
        {
            category = new Category();
            category->name = categoryName;
            categories.push_back(category);
        }

        category->values[name] = std::to_string(value);

        if (saveOnValuesChange)
            Save();
    }

private:
    Config() = default;

    std::string iniFileName = std::string(PROJECT_LABEL) + ".ini";
    std::vector<Category *> categories;
    Category *findCategory(std::string category);
    std::string trimString(std::string string);
    bool saveOnValuesChange = false;
};
