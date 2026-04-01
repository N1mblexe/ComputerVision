#include "ConfigManager.hpp"

ConfigManager::ConfigManager(const std::string& path) : filePath(path) {
    Load();
}

void ConfigManager::Load() {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filePath << std::endl;
        return;
    }

    std::string key;
    double value;
    while (file >> key >> value) {
        settings[key] = value;
    }
    file.close();
}

double ConfigManager::Get(const std::string& key, double defaultValue) const {
    auto it = settings.find(key);
    if (it != settings.end()) {
        return it->second;
    }
    return defaultValue;
}