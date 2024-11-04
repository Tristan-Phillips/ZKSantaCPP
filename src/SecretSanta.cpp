#include "SecretSanta.h"
#include "SMTPClient.h"
#include <algorithm>
#include <random>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

SecretSanta::SecretSanta(const std::vector<std::pair<std::string, std::string>>& participants)
    : m_participants(participants) {
    std::cout << "SecretSanta initialized with " << participants.size() << " participants." << std::endl;
}

void SecretSanta::shuffle() {
    std::vector<std::pair<std::string, std::string>> shuffled = m_participants;
    std::random_device rd;
    std::mt19937 g(rd());

    bool validShuffle = false;
    while (!validShuffle) {
        std::shuffle(shuffled.begin(), shuffled.end(), g);

        // Check that no one is assigned to themselves
        validShuffle = true;
        for (size_t i = 0; i < shuffled.size(); ++i) {
            if (shuffled[i].first == m_participants[i].first) {
                validShuffle = false;
                break;
            }
        }
    }

    // Assign shuffled results to the member map
    for (size_t i = 0; i < m_participants.size(); ++i) {
        m_santaPairs[m_participants[i].first] = shuffled[i].first;
    }

    std::cout << "Participants shuffled successfully." << std::endl;
}

void SecretSanta::sendEmails() {
    std::ifstream env_file("../.env");
    std::string smtp_server, smtp_port_str, smtp_user, smtp_pass;

    if (!env_file.is_open()) {
        std::cerr << "Failed to open .env file for SMTP settings." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(env_file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            if (key == "SMTP_SERVER") smtp_server = value;
            else if (key == "SMTP_PORT") smtp_port_str = value;
            else if (key == "SMTP_USER") smtp_user = value;
            else if (key == "SMTP_PASS") smtp_pass = value;
        }
    }

    if (smtp_server.empty() || smtp_port_str.empty() || smtp_user.empty() || smtp_pass.empty()) {
        std::cerr << "Incomplete SMTP settings in .env file." << std::endl;
        return;
    }

    int smtp_port;
    try {
        smtp_port = std::stoi(smtp_port_str);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid SMTP port number in .env file: " << smtp_port_str << std::endl;
        return;
    }

    SMTPClient client(smtp_server, smtp_port, smtp_user, smtp_pass);

    for (const auto& [giver, email] : m_participants) {
        std::string receiver = m_santaPairs[giver];
        std::string body = generateEmailBody(giver, receiver);
        if (client.sendEmail(email, "Your Secret Santa Assignment", body)) {
            std::cout << "Email sent to " << email << " successfully." << std::endl;
        } else {
            std::cerr << "Failed to send email to " << email << "." << std::endl;
        }
    }
}

void SecretSanta::saveResultsToFile(const std::string& filename) {
    nlohmann::json json_result;
    for (const auto& pair : m_santaPairs) {
        json_result[pair.first] = pair.second;
    }
    std::ofstream file(filename);
    if (file.is_open()) {
        file << json_result.dump(4);
        std::cout << "Results saved to " << filename << "." << std::endl;
    } else {
        std::cerr << "Failed to open file " << filename << " for writing results." << std::endl;
    }
}

std::string SecretSanta::generateEmailBody(const std::string& giver, const std::string& receiver) {
    std::cout << "Generating email body for " << giver << " who is the Secret Santa for " << receiver << "." << std::endl;
    return "<html><body><h1>Secret Santa</h1><p>Hi " + giver + ",</p><p>You are the Secret Santa for " + receiver + ".</p></body></html>";
}
