#ifndef SECRETSANTA_H
#define SECRETSANTA_H

#include <vector>
#include <string>
#include <map>

class SecretSanta {
public:
    SecretSanta(const std::vector<std::pair<std::string, std::string>>& participants);
    void shuffle();
    void sendEmails();
    void saveResultsToFile(const std::string& filename);

private:
    std::vector<std::pair<std::string, std::string>> m_participants;
    std::map<std::string, std::string> m_santaPairs;

    std::string generateEmailBody(const std::string& giver, const std::string& receiver);
};

#endif // SECRETSANTA_H