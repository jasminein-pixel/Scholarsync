#include "auth.h"
std::string auth::sha256(std::string input)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input.c_str(), input.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return ss.str();
}

// Generate a random 16-byte salt → 32 char hex string
std::string auth::generateSalt()
{
    unsigned char salt[16];
    RAND_bytes(salt, 16);

    std::stringstream ss;
    for (int i = 0; i < 16; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)salt[i];
    return ss.str();
}

std::string auth::hashPassword(std::string password)
{
    std::string salt = generateSalt();
    std::string hash = sha256(salt + password);
    return salt + ":" + hash; // stored as one string in DB
}

bool auth::verifyPassword(std::string inputPassword, std::string storedValue)
{
    std::string salt = storedValue.substr(0, 32);    // first 32 chars = salt
    std::string storedHash = storedValue.substr(33); // after ":" = hash
    std::string inputHash = sha256(salt + inputPassword);
    return inputHash == storedHash;
}
