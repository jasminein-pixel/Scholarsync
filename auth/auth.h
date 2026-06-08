#pragma once
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <string>

class auth
{

private:
    std::string sha256(std::string input);
    std::string generateSalt();
public:
    std::string hashPassword(std::string password);
    bool verifyPassword(std::string inputPassword, std::string storedValue);
};