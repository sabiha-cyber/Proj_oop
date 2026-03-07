#ifndef AUTHENTICATION_SERVICE_H
#define AUTHENTICATION_SERVICE_H

#include "User.h"
#include <vector>
#include <map>
#include <string>

class AuthenticationService {
private:
    std::vector<User>            users;
    std::map<std::string, int>   usernameToUserId;
    int                          nextUserId = 1;

    static const std::string USERS_FILE;

    void loadUsersFromFile();
    void saveUsersToFile() const;

public:
    AuthenticationService();

    // ── Registration & Login ──────────────────────────────────────────────────
    bool registerUser(const std::string& username, const std::string& password);
    int  login       (const std::string& username, const std::string& password) const;

    // ── Lookup (non-const + const overloads) ──────────────────────────────────
    User*       findUserById      (int id);
    const User* findUserById      (int id)                     const;

    User*       findUserByUsername(const std::string& username);
    const User* findUserByUsername(const std::string& username) const;

    // ── Display ───────────────────────────────────────────────────────────────
    void listAllUsers() const;

    // ── Accessors for FriendService / SocialNetwork ───────────────────────────
    std::vector<User>&              getUsers();
    const std::vector<User>&        getUsers()             const;   // ← ADDED

    std::map<std::string, int>&       getUsernameToIdMap();
    const std::map<std::string, int>& getUsernameToIdMap() const;   // ← ADDED
};

#endif // AUTHENTICATION_SERVICE_H
