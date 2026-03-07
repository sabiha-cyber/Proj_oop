#include "AuthenticationService.h"
#include <fstream>
#include <iostream>
#include <algorithm>

const std::string AuthenticationService::USERS_FILE = "users.txt";

// ── Constructor ───────────────────────────────────────────────────────────────

AuthenticationService::AuthenticationService() {

    users.reserve(1024);
    loadUsersFromFile();
}



void AuthenticationService::loadUsersFromFile() {
    std::ifstream in(USERS_FILE);
    if (!in.is_open()) {
        std::cout << "No user file found at '" << USERS_FILE
                  << "'. Starting fresh.\n";
        return;
    }

    int         id;
    std::string uname, pass;
    int         loaded = 0;

    while (in >> id >> uname >> pass) {
        users.emplace_back(id, uname, pass);
        usernameToUserId[uname] = id;
        nextUserId = std::max(nextUserId, id + 1);
        ++loaded;
    }
    in.close();
    std::cout << loaded << " user(s) loaded from '" << USERS_FILE << "'.\n";
}

void AuthenticationService::saveUsersToFile() const {
    std::ofstream out(USERS_FILE);
    if (!out.is_open()) {
        std::cout << "❌ Could not open '" << USERS_FILE << "' for writing.\n";
        return;
    }
    for (const auto& u : users) {
        out << u.getUserId()   << " "
            << u.getUsername() << " "
            << u.getPassword() << "\n";
    }
    out.close();
}

// ── Registration & Login ──────────────────────────────────────────────────────

bool AuthenticationService::registerUser(const std::string& username,
                                          const std::string& password) {
    // Validate username
    if (username.empty() || username.find(' ') != std::string::npos) {
        std::cout << "❌ Invalid username (no spaces allowed).\n";
        return false;
    }
    if (username.length() < 3) {                          // ← ADDED: min-length
        std::cout << "❌ Username too short (minimum 3 characters).\n";
        return false;
    }

    // Validate password
    if (password.length() < 8) {
        std::cout << "❌ Password too short (minimum 8 characters).\n";
        return false;
    }

    // Duplicate check
    if (usernameToUserId.count(username)) {
        std::cout << "❌ Username already exists.\n";
        return false;
    }

    // Guard: if we somehow approach the reserved capacity, extend it safely
    // before any pointer is taken (no pointer is held at this call site).
    if (users.size() >= users.capacity()) {
        users.reserve(users.capacity() * 2);
    }

    int newId = nextUserId++;
    users.emplace_back(newId, username, password);
    usernameToUserId[username] = newId;
    saveUsersToFile();

    std::cout << "✅ User created: " << username << " (ID " << newId << ")\n";
    return true;
}

int AuthenticationService::login(const std::string& username,
                                  const std::string& password) const {
    auto it = usernameToUserId.find(username);
    if (it == usernameToUserId.end()) {
        std::cout << "❌ User not found.\n";
        return -1;
    }

    int id = it->second;
    for (const auto& u : users) {
        if (u.getUserId() == id) {
            if (u.getPassword() == password) {
                std::cout << "✅ Login successful: " << username << "\n";
                return id;
            }
            std::cout << "❌ Incorrect password.\n";
            return -1;
        }
    }

    std::cout << "❌ User not found.\n";
    return -1;
}

// ── Lookup ────────────────────────────────────────────────────────────────────

User* AuthenticationService::findUserById(int id) {
    for (auto& u : users)
        if (u.getUserId() == id) return &u;
    return nullptr;
}

const User* AuthenticationService::findUserById(int id) const {
    for (const auto& u : users)
        if (u.getUserId() == id) return &u;
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

// ── Display ───────────────────────────────────────────────────────────────────

void AuthenticationService::listAllUsers() const {
    std::cout << "\n╔═══════════ Registered Users ═══════════╗\n";
    if (users.empty()) {
        std::cout << "║  No users registered yet.              ║\n";
    } else {
        for (const auto& u : users) {
            std::cout << "║  ID: " << u.getUserId()
                      << " | @" << u.getUsername()
                      << " | Friends: " << u.getFriendIds().size() << "\n";
        }
    }
    std::cout << "╚════════════════════════════════════════╝\n";
}

// ── Accessors ─────────────────────────────────────────────────────────────────

std::vector<User>& AuthenticationService::getUsers() {
    return users;
}

const std::vector<User>& AuthenticationService::getUsers() const {   // ← ADDED
    return users;
}

std::map<std::string, int>& AuthenticationService::getUsernameToIdMap() {
    return usernameToUserId;
}

const std::map<std::string, int>&                                      // ← ADDED
AuthenticationService::getUsernameToIdMap() const {
    return usernameToUserId;
}
