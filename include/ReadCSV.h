#ifndef READCSV_H
#define READCSV_H

#include <vector>
#include <string>

struct Participant {
    std::string name;
    std::string email;
};

class ReadCSV {
public:
    static std::vector<Participant> readParticipantsFromCSV(const std::string& filename);
};

#endif // READCSV_H