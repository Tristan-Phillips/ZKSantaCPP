#include "EnvReader.h"
#include <fstream>
#include <sstream>
#include <iostream>

EnvReader::EnvReader(const std::string &filepath) {
    loadEnv(filepath);
}

void EnvReader::loadEnv(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open .env file");
    }
    std::string line;
    while (std::getline(file, line)) {
        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            m_envVariables[key] = value;
        }
    }
    file.close();
}

std::unordered_map<std::string, std::string> EnvReader::getEnvVariables() const {
    return m_envVariables;
}