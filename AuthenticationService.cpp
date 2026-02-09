#include "AuthenticationService.h"
#include <fstream>
#include <iostream>
#include <algorithm>

const std::string AuthenticationService::USERS_FILE = "users.txt";

AuthenticationService::AuthenticationService() {
    loadUsersFromFile();
}

void AuthenticationService::loadUsersFromFile() {
    std::ifstream in(USERS_FILE);
    if (!in.is_open()) return;

    int id;
    std::string u, p;
    while (in >> id >> u >> p) {
        users.emplace_back(id, u, p);
        usernameToUserId[u] = id;
        nextUserId = std::max(nextUserId, id + 1);
    }
    in.close();
}

void AuthenticationService::saveUsersToFile() const {
    std::ofstream out(USERS_FILE);
    if (!out.is_open()) return;

    for (const auto& user : users) {
        out << user.getUserId() << " "
            << user.getUsername() << " "
            << user.getPassword() << "\n";
    }
    out.close();
}

bool AuthenticationService::registerUser(const std::string& username, const std::string& password) {
    if (username.empty() || username.find(' ') != std::string::npos) {
        std::cout << "Invalid username.\n";
        return false;
    }
    if (password.length() < 8) {
        std::cout << "Password too short (min 8 chars).\n";
        return false;
    }
    if (usernameToUserId.count(username)) {
        std::cout << "Username already exists.\n";
        return false;
    }

    int newId = nextUserId++;
    users.emplace_back(newId, username, password);
    usernameToUserId[username] = newId;
    saveUsersToFile();

    std::cout << "User created: " << username << " (ID " << newId << ")\n";
    return true;
}

int AuthenticationService::login(const std::string& username, const std::string& password) const {
    auto it = usernameToUserId.find(username);
    if (it == usernameToUserId.end()) {
        std::cout << "User not found.\n";
        return -1;
    }

    int id = it->second;
    for (const auto& u : users) {
        if (u.getUserId() == id && u.getPassword() == password) {
            std::cout << "Login successful: " << username << "\n";
            return id;
        }
    }
    std::cout << "Incorrect password.\n";
    return -1;
}

User* AuthenticationService::findUserById(int id) {
    for (auto& u : users) {
        if (u.getUserId() == id) return &u;
    }
    return nullptr;
}

const User* AuthenticationService::findUserById(int id) const {
    for (const auto& u : users) {
        if (u.getUserId() == id) return &u;
    }
    return nullptr;
}

User* AuthenticationService::findUserByUsername(const std::string& username) {
    auto it = usernameToUserId.find(username);
    if (it == usernameToUserId.end()) return nullptr;
    return findUserById(it->second);
}

const User* AuthenticationService::findUserByUsername(const std::string& username) const {
    auto it = usernameToUserId.find(username);
    if (it == usernameToUserId.end()) return nullptr;
    return findUserById(it->second);
}

void AuthenticationService::listAllUsers() const {
    std::cout << "\nRegistered users:\n";
    for (const auto& u : users) {
        u.printBasicInfo();
    }
}

std::vector<User>& AuthenticationService::getUsers() {
    return users;
}

std::map<std::string, int>& AuthenticationService::getUsernameToIdMap() {
    return usernameToUserId;
}