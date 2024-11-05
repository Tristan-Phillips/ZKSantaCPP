#ifndef ENVREADER_H
#define ENVREADER_H

#include <string>
#include <vector>
#include <unordered_map>

class EnvReader {
public:
    explicit EnvReader(const std::string &filepath);
    std::unordered_map<std::string, std::string> getEnvVariables() const;
    
private:
    std::unordered_map<std::string, std::string> m_envVariables;
    void loadEnv(const std::string &filepath);
};

#endif // ENVREADER_H