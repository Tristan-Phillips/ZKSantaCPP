#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "EnvReader.h"
#include "SecretSanta.h"
#include "EmailSender.h"

int main() {
    try {
        EnvReader envReader("../.env");
        auto envVariables = envReader.getEnvVariables();

        std::vector<std::string> participants;
        std::istringstream ss(envVariables["PARTICIPANTS"]);
        std::string participant;
        while (std::getline(ss, participant, ',')) {
            participants.push_back(participant);
        }

        SecretSanta secretSanta(participants);
        auto assignments = secretSanta.assign();

        EmailSender emailSender(
            envVariables["SMTP_SERVER"],
            std::stoi(envVariables["SMTP_PORT"]),
            envVariables["SMTP_USERNAME"],
            envVariables["SMTP_PASSWORD"]
        );

        for (const auto& [giver, receiver] : assignments) {
            std::string subject = "Your Secret Santa Assignment";
            std::string body = "Hi " + giver + ",\n\nYou are the Secret Santa for " + receiver + ".\n\nHappy Holidays!";
            emailSender.sendEmail(envVariables["FROM_EMAIL"], giver, subject, body);
        }

        nlohmann::json resultJson(assignments);
        std::ofstream resultFile("results.json");
        resultFile << resultJson.dump(4);
        resultFile.close();

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}