#ifndef EMAILSENDER_H
#define EMAILSENDER_H

#include <string>

class EmailSender {
public:
    EmailSender(const std::string &server, int port, const std::string &username, const std::string &password);
    void sendEmail(const std::string &from, const std::string &to, const std::string &subject, const std::string &body);

private:
    std::string m_server;
    int m_port;
    std::string m_username;
    std::string m_password;

    std::string Base64Encode(const std::string &input);
};

#endif // EMAILSENDER_H