#ifndef AUTHENTICATION_SERVICE_H
#define AUTHENTICATION_SERVICE_H

#include "User.h"
#include <vector>
#include <map>
#include <string>

class AuthenticationService {
private:
    std::vector<User> users;
    std::map<std::string, int> usernameToUserId;

    int nextUserId = 1;

    static const std::string USERS_FILE;

    void loadUsersFromFile();
    void saveUsersToFile() const;

public:
    AuthenticationService();

    bool registerUser(const std::string& username, const std::string& password);

    int login(const std::string& username, const std::string& password) const;

    User*       findUserById(int id);
    const User* findUserById(int id) const;

    User*       findUserByUsername(const std::string& username);
    const User* findUserByUsername(const std::string& username) const;

    void listAllUsers() const;

    // Allow FriendService / SocialNetwork to access users
    std::vector<User>&       getUsers();
    std::map<std::string, int>& getUsernameToIdMap();
};

#endif // AUTHENTICATION_SERVICE_H