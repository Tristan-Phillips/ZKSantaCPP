#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "SecretSanta.h"
#include <nlohmann/json.hpp>

int main() {
    std::cout << "Initializing participants for Secret Santa." << std::endl;

    std::vector<std::pair<std::string, std::string>> participants = {
        {"a", "a@a.com"},
        {"b", "b@b.com"},
        {"c", "c@c.com"},
        {"d", "d@d.com"}
    };

    if (participants.empty()) {
        std::cerr << "No participants found. Exiting..." << std::endl;
        return 1;
    }

    SecretSanta secretSanta(participants);
    std::cout << "Shuffling participants..." << std::endl;
    secretSanta.shuffle();

    std::cout << "Sending emails to participants..." << std::endl;
    secretSanta.sendEmails();

    std::cout << "Saving results to result.json..." << std::endl;
    secretSanta.saveResultsToFile("result.json");

    std::cout << "Secret Santa emails have been sent and results saved!" << std::endl;

    return 0;
}