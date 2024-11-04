cmake_minimum_required(VERSION 3.10)
project(SecretSanta)

set(CMAKE_CXX_STANDARD 17)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(include)

# Add executable
add_executable(SecretSanta
    src/main.cpp
    src/SecretSanta.cpp
    src/SMTPClient.cpp
)

# Include and link OpenSSL
target_include_directories(SecretSanta PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(SecretSanta PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Add nlohmann/json as a dependency
# Assuming nlohmann/json is installed via package manager
find_package(nlohmann_json REQUIRED)
target_link_libraries(SecretSanta PRIVATE nlohmann_json::nlohmann_json)