#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

class SMTPClient {
public:
    SMTPClient(const std::string& server, int port, const std::string& user, const std::string& pass);
    ~SMTPClient();
    bool sendEmail(const std::string& to, const std::string& subject, const std::string& body);

private:
    std::string m_server;
    int m_port;
    std::string m_user;
    std::string m_pass;
    SSL_CTX* m_ctx;
    SSL* m_ssl;
    BIO* m_bio;

    bool initializeSSL();
    void cleanupSSL();
    bool connect();
    bool authenticate();
    bool sendCommand(const std::string& cmd, const std::string& expected);
    std::string encodeBase64(const std::string& input); // Added declaration for base64 encoding
};

#endif // SMTPCLIENT_H