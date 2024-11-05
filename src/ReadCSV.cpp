#include "ReadCSV.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::vector<Participant> ReadCSV::readParticipantsFromCSV(const std::string& filename) {
    std::vector<Participant> participants;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open participants file");
    }

    std::string line, name, email;
    // Ignore the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        if (std::getline(ss, name, ',') && std::getline(ss, email, ',')) {
            participants.push_back({name, email});
        }
    }

    file.close();
    return participants;
}
