#ifndef CONFIG_MANAGER
#define CONFIG_MANAGER

#include <string>
#include <map>
#include <fstream>
#include <iostream>

class ConfigManager {
private:
    std::map<std::string, double> settings;
    std::string filePath;

public:
    ConfigManager(const std::string& path);
    
    void Load();

    double Get(const std::string& key, double defaultValue = 0.0) const;
};

#endif