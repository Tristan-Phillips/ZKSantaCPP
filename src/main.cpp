#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include "EnvReader.h"
#include "SecretSanta.h"
#include "EmailSender.h"
#include "ReadCSV.h"

int main() {
    try {
        // Load environment variables
        EnvReader envReader("../.env");
        auto envVariables = envReader.getEnvVariables();

        // Read participants from CSV
        std::string csvFilePath = "../participants.csv";
        std::vector<Participant> participants = ReadCSV::readParticipantsFromCSV(csvFilePath);

        if (participants.empty()) {
            throw std::runtime_error("No participants found in CSV file.");
        }

        // Prepare participants' emails and names for assignments
        std::vector<std::string> participantNames;
        std::unordered_map<std::string, std::string> emailMap;
        for (const auto& participant : participants) {
            std::cout << "Loaded Participant: " << participant.name << ", Email: " << participant.email << std::endl;
            participantNames.push_back(participant.name);
            emailMap[participant.name] = participant.email;
        }

        // Create Secret Santa assignments
        SecretSanta secretSanta(participantNames);
        auto assignments = secretSanta.assign();

        // Initialize email sender
        EmailSender emailSender(
            envVariables["SMTP_SERVER"],
            std::stoi(envVariables["SMTP_PORT"]),
            envVariables["SMTP_USERNAME"],
            envVariables["SMTP_PASSWORD"]
        );

        // Send emails to each participant
        for (const auto& [giver, receiver] : assignments) {
            std::string subject = "Your Secret Santa Assignment";
            std::string body = "Hi " + giver + ",\n\nYou are the Secret Santa for " + receiver + ".\n\nHappy Holidays!";
            std::string toEmail = emailMap[giver];
            std::cout << "Sending email to " << giver << " at " << toEmail << std::endl;
            emailSender.sendEmail(envVariables["FROM_EMAIL"], toEmail, subject, body);
        }

        // Write assignments to JSON file
        nlohmann::json resultJson(assignments);
        std::ofstream resultFile("results.json");
        resultFile << resultJson.dump(4);
        resultFile.close();
        std::cout << "Assignments saved to results.json" << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}