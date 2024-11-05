# ZKSantaCPP

(DISCLAIMER its not ZK, at least not yet)

This project is a memory-secure, zero-knowledge Secret Santa application written in C++. 
The program assigns Secret Santa pairs to participants and sends them their assignments via email using SMTP with SSL/TLS authentication. 
Sensitive information is stored in a `.env` file, and the results are logged in a `results.json` file.

## Features

- **Secure Email Sending**: Utilizes SSL/TLS for secure communication with SMTP servers.
- **Zero-Knowledge Assignment**: Participants only receive their own Secret Santa assignment.
- **Environment Configuration**: Easy configuration through a `.env` file.
- **JSON Logging**: Outputs the assignment results to a `results.json` file.

## Project Structure
ZKSantaCPP/
│
├── include/
│ ├── EnvReader.h
│ ├── SecretSanta.h
│ ├── EmailSender.h
│ └── ReadCSV.h
│
├── src/
│ ├── EnvReader.cpp
│ ├── SecretSanta.cpp
│ ├── EmailSender.cpp
│ ├── main.cpp
│ └── ReadCSV.cpp
│
├── utils/
│ └── test-smtp.sh
│
├── participants.csv
├── CMakeLists.txt
├── .env
└── results.json

## Getting Started

### Prerequisites

- **C++17** or higher
- **CMake** 3.10 or higher
- **OpenSSL** for SSL/TLS support
- **nlohmann/json** library for JSON handling

## Configure the project
`mkdir build`
`cd build`
`cmake ..`
`make`

## Configuration

To configure the Secret Santa program, you need to create a `.env` file in the root directory of the project. This file will store all the sensitive information required by the application. Here’s how you can set it up:

### Step-by-Step Setup

1. **Create a `.env` file** in the root directory of the project.

2. **Add the following configuration details** (replace the placeholder values with your actual information):

   ```plaintext
   SMTP_SERVER=smtp.example.com
   SMTP_PORT=465
   SMTP_USERNAME=your_username
   SMTP_PASSWORD=your_password
   PARTICIPANTS=alice@example.com,bob@example.com,charlie@example.com
   FROM_EMAIL=from@example.com
   ```

### CSV File

The participants.csv file should be placed in the root directory and must have the following format:
(No spaces before or after comma / not fixed yet)

    ```plaintext
    Full Name,Email
    Alice Johnson,alice@example.com
    Bob Smith,bob@example.com
    Charlie Brown,charlie@example.com
    ```

## Running the program
`./SecretSanta`

# Acknowledgements
- Uses [OpenSSL](https://www.openssl.org/) for secure network communication.
- JSON handling by [nlohmann/json](https://github.com/nlohmann/json).

### Important Notes

- **Security**: Ensure your `.env` file is kept secure and is not committed to version control systems.
- **SMTP Configuration**: Ensure your SMTP server supports SSL/TLS connections and that the credentials provided are correct.
- **Testing**: Test the program with a small set of emails to ensure it functions as expected before scaling up.