#include "EmailSender.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

EmailSender::EmailSender(const std::string &server, int port, const std::string &username, const std::string &password)
    : m_server(server), m_port(port), m_username(username), m_password(password) {}

void EmailSender::sendEmail(const std::string &from, const std::string &to, const std::string &subject, const std::string &body) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD *method = SSLv23_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (ctx == nullptr) {
        ERR_print_errors_fp(stderr);
        throw std::runtime_error("Unable to create SSL context");
    }

    struct hostent *host = gethostbyname(m_server.c_str());
    if (host == nullptr) {
        SSL_CTX_free(ctx);
        throw std::runtime_error("Unable to resolve SMTP server address");
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        SSL_CTX_free(ctx);
        throw std::runtime_error("Unable to create socket");
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_port);
    server_addr.sin_addr = *((struct in_addr*)host->h_addr);
    memset(&(server_addr.sin_zero), '\0', 8);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        close(sockfd);
        SSL_CTX_free(ctx);
        throw std::runtime_error("Cannot connect to the SMTP server");
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        close(sockfd);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        throw std::runtime_error("SSL connection failed");
    }

    auto send_command = [&](const std::string &cmd) {
        std::cout << "C: " << cmd << std::endl;
        if (SSL_write(ssl, cmd.c_str(), cmd.size()) <= 0) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to send command to SMTP server");
        }
    };

    auto read_response = [&]() -> std::string {
        char buffer[1024];
        int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (bytes < 0) {
            ERR_print_errors_fp(stderr);
            throw std::runtime_error("Failed to read response from SMTP server");
        }
        buffer[bytes] = '\0';
        std::string response(buffer);
        std::cout << "S: " << response << std::endl;
        return response;
    };

    // Read initial server response
    read_response();

    // Send EHLO
    send_command("EHLO localhost\r\n");
    read_response();

    // Authenticate
    send_command("AUTH LOGIN\r\n");
    read_response();

    send_command(Base64Encode(m_username) + "\r\n");
    read_response();

    send_command(Base64Encode(m_password) + "\r\n");
    read_response();

    // Send MAIL FROM
    send_command("MAIL FROM: <" + from + ">\r\n");
    read_response();

    // Send RCPT TO
    send_command("RCPT TO: <" + to + ">\r\n");
    read_response();

    // Send DATA
    send_command("DATA\r\n");
    read_response();

    // Send email content
    std::string emailContent = "Subject: " + subject + "\r\n\r\n" + body + "\r\n.\r\n";
    send_command(emailContent);
    read_response();

    // Close connection
    send_command("QUIT\r\n");
    read_response();

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
}

std::string EmailSender::Base64Encode(const std::string &input) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Do not use newlines to flush buffer
    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    std::string encodedData(bufferPtr->data, bufferPtr->length);
    BUF_MEM_free(bufferPtr);
    return encodedData;
}