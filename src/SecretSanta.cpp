#include "SecretSanta.h"
#include <algorithm>
#include <random>
#include <chrono>

SecretSanta::SecretSanta(const std::vector<std::string>& participants)
    : m_participants(participants) {}

void SecretSanta::shuffleParticipants() {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(m_participants.begin(), m_participants.end(), std::default_random_engine(seed));
}

std::unordered_map<std::string, std::string> SecretSanta::assign() {
    shuffleParticipants();
    size_t n = m_participants.size();
    for (size_t i = 0; i < n; ++i) {
        m_assignments[m_participants[i]] = m_participants[(i + 1) % n];
    }
    return m_assignments;
}