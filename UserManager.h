#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <vector>
#include <string>
#include <map>

class User;

class UserManager {
private:
    std::vector<User*> users;
    std::map<std::string, int> usernameToUserId;
    int nextUserId;

    void updateUsernameMap();

public:
    UserManager();
    ~UserManager();

    void clear();

    // Authentication
    bool registerUser(const std::string& username, const std::string& password);
    int login(const std::string& username, const std::string& password) const;

    // Creation (for internal use or loading)
    User* createAndAdd(const std::string& username, const std::string& password);
    User* createAndAdd(const std::string& username, const std::string& password, int explicitId);

    // Lookup
    User* findUserById(int id) const;
    User* findUserByUsername(const std::string& username) const;
    const std::vector<User*>& getAllUsers() const;

    // Deletion
    bool deleteUserById(int id);
    bool deleteUserByUsername(const std::string& username);

    // Persistence
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);

    // Display
    void listAllUsers() const;

    // Stats
    size_t getUserCount() const;
    int getNextAvailableId() const;
};

#endif