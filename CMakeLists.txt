cmake_minimum_required(VERSION 3.10)
project(SecretSanta)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(include)

add_executable(SecretSanta
    src/main.cpp
    src/EnvReader.cpp
    src/SecretSanta.cpp
    src/EmailSender.cpp
    src/ReadCSV.cpp
)

target_link_libraries(SecretSanta OpenSSL::SSL nlohmann_json::nlohmann_json)