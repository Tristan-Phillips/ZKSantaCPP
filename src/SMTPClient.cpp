#include "SMTPClient.h"
#include <iostream>
#include <sstream>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>

SMTPClient::SMTPClient(const std::string& server, int port, const std::string& user, const std::string& pass)
    : m_server(server), m_port(port), m_user(user), m_pass(pass), m_ctx(nullptr), m_ssl(nullptr), m_bio(nullptr) {
    std::cout << "Initializing SMTP client for server: " << server << " on port: " << port << std::endl;
    if (!initializeSSL()) {
        std::cerr << "Failed to initialize SSL." << std::endl;
    } else {
        std::cout << "SSL initialized successfully." << std::endl;
    }
}

SMTPClient::~SMTPClient() {
    cleanupSSL();
}

bool SMTPClient::initializeSSL() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    m_ctx = SSL_CTX_new(TLS_client_method());
    if (!m_ctx) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    return true;
}

void SMTPClient::cleanupSSL() {
    if (m_ssl) SSL_free(m_ssl);
    if (m_bio) BIO_free_all(m_bio);
    if (m_ctx) SSL_CTX_free(m_ctx);
}

bool SMTPClient::connect() {
    std::stringstream ss;
    ss << m_server << ":" << m_port;
    m_bio = BIO_new_ssl_connect(m_ctx);
    if (!m_bio) {
        std::cerr << "Failed to create BIO connection." << std::endl;
        return false;
    }

    BIO_get_ssl(m_bio, &m_ssl);
    SSL_set_mode(m_ssl, SSL_MODE_AUTO_RETRY);

    BIO_set_conn_hostname(m_bio, ss.str().c_str());
    std::cout << "Connecting to SMTP server: " << ss.str() << std::endl;
    if (BIO_do_connect(m_bio) <= 0) {
        std::cerr << "Connection to SMTP server failed." << std::endl;
        ERR_print_errors_fp(stderr);
        return false;
    }
    std::cout << "Connected to SMTP server successfully." << std::endl;

    char buf[512];
    if (BIO_read(m_bio, buf, sizeof(buf) - 1) <= 0) {
        std::cerr << "Failed to read initial server response." << std::endl;
        ERR_print_errors_fp(stderr);
        return false;
    }
    buf[sizeof(buf) - 1] = '\0'; // Ensure null-termination
    std::cout << "Initial server response: " << buf << std::endl;

    return true;
}

bool SMTPClient::authenticate() {
    std::cout << "Authenticating with SMTP server." << std::endl;
    if (!sendCommand("EHLO localhost", "250")) {
        std::cerr << "EHLO command failed." << std::endl;
        return false;
    }

    if (!sendCommand("AUTH LOGIN", "334")) {
        std::cerr << "AUTH LOGIN command failed." << std::endl;
        return false;
    }

    std::string encoded_user = encodeBase64(m_user);
    if (!sendCommand(encoded_user, "334")) {
        std::cerr << "Sending username failed." << std::endl;
        return false;
    }

    std::string encoded_pass = encodeBase64(m_pass);
    if (!sendCommand(encoded_pass, "235")) {
        std::cerr << "Sending password failed." << std::endl;
        return false;
    }

    std::cout << "Authenticated successfully with SMTP server." << std::endl;
    return true;
}

bool SMTPClient::sendCommand(const std::string& cmd, const std::string& expected) {
    std::cout << "Sending command: " << cmd << std::endl;
    BIO_printf(m_bio, "%s\r\n", cmd.c_str());
    char buf[512];
    if (BIO_read(m_bio, buf, sizeof(buf) - 1) > 0) {
        buf[sizeof(buf) - 1] = '\0'; // Ensure null-termination
        std::string response(buf);
        std::cout << "Server response: " << response << std::endl;
        return response.find(expected) != std::string::npos;
    }
    std::cerr << "Failed to read server response for command: " << cmd << std::endl;
    return false;
}

bool SMTPClient::sendEmail(const std::string& to, const std::string& subject, const std::string& body) {
    std::cout << "Preparing to send email to: " << to << std::endl;
    if (!connect() || !authenticate()) {
        std::cerr << "Failed to connect or authenticate." << std::endl;
        return false;
    }
    
    std::string from = "MAIL FROM:<" + m_user + ">\r\n";
    std::string rcpt = "RCPT TO:<" + to + ">\r\n";
    std::string data = "DATA\r\n";
    std::string subj = "Subject: " + subject + "\r\n";
    std::string content = "Content-Type: text/html\r\n\r\n" + body + "\r\n.\r\n";

    if (!sendCommand(from, "250") ||
        !sendCommand(rcpt, "250") ||
        !sendCommand(data, "354") ||
        !sendCommand(subj + content, "250") ||
        !sendCommand("QUIT", "221")) {
        std::cerr << "Failed to send email to " << to << std::endl;
        return false;
    }

    std::cout << "Email sent successfully to " << to << "." << std::endl;
    return true;
}

std::string SMTPClient::encodeBase64(const std::string& input) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Do not use newlines to flush buffer
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return result;
}