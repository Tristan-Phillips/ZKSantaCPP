#ifndef SECRETSANTA_H
#define SECRETSANTA_H

#include <vector>
#include <string>
#include <unordered_map>

class SecretSanta {
public:
    explicit SecretSanta(const std::vector<std::string>& participants);
    std::unordered_map<std::string, std::string> assign();

private:
    std::vector<std::string> m_participants;
    std::unordered_map<std::string, std::string> m_assignments;
    void shuffleParticipants();
};

#endif // SECRETSANTA_H