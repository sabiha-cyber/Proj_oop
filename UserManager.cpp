#include "UserManager.h"
#include "User.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

// ── Constructor / Destructor ──────────────────────────────────────────────────

UserManager::UserManager() : nextUserId(1) {}

UserManager::~UserManager() {
    clear();
}

void UserManager::clear() {
    for (User* u : users) {
        delete u;
    }
    users.clear();
    usernameToUserId.clear();
    nextUserId = 1;
}

void UserManager::updateUsernameMap() {
    usernameToUserId.clear();
    for (User* u : users) {
        if (u) {
            usernameToUserId[u->getUsername()] = u->getUserId();
        }
    }
}

// ── Authentication ────────────────────────────────────────────────────────────

bool UserManager::registerUser(const string& username, const string& password) {
    // Validate username
    if (username.empty() || username.find(' ') != string::npos) {
        cout << "❌ Invalid username (no spaces allowed).\n";
        return false;
    }
    
    if (username.length() < 3) {
        cout << "❌ Username too short (minimum 3 characters).\n";
        return false;
    }
    
    // Validate password
    if (password.length() < 4) {
        cout << "❌ Password too short (minimum 4 characters).\n";
        return false;
    }
    
    // Check if username already exists
    if (usernameToUserId.count(username)) {
        cout << "❌ Username already exists.\n";
        return false;
    }
    
    // Create new user
    int newId = nextUserId++;
    User* newUser = new User(newId, username, password);
    users.push_back(newUser);
    usernameToUserId[username] = newId;
    
    cout << "✅ User created: " << username << " (ID " << newId << ")\n";
    return true;
}

int UserManager::login(const string& username, const string& password) const {
    auto it = usernameToUserId.find(username);
    if (it == usernameToUserId.end()) {
        cout << "❌ User not found.\n";
        return -1;
    }
    
    int id = it->second;
    for (const User* u : users) {
        if (u && u->getUserId() == id) {
            if (u->getPassword() == password) {
                cout << "✅ Login successful: " << username << "\n";
                return id;
            } else {
                cout << "❌ Incorrect password.\n";
                return -1;
            }
        }
    }
    
    cout << "❌ User not found.\n";
    return -1;
}

// ── Creation ──────────────────────────────────────────────────────────────────

User* UserManager::createAndAdd(const string& username, const string& password) {
    if (username.empty() || password.empty()) {
        cout << "Error: Username and password cannot be empty.\n";
        return nullptr;
    }

    if (findUserByUsername(username) != nullptr) {
        cout << "Error: Username '" << username << "' already exists.\n";
        return nullptr;
    }

    User* u = new User(nextUserId, username, password);
    users.push_back(u);
    usernameToUserId[username] = nextUserId;
    ++nextUserId;
    return u;
}

User* UserManager::createAndAdd(const string& username, const string& password, int explicitId) {
    if (explicitId < 1) {
        cout << "Error: Invalid user ID.\n";
        return nullptr;
    }

    if (findUserById(explicitId) != nullptr) {
        cout << "Error: User ID " << explicitId << " already in use.\n";
        return nullptr;
    }

    if (findUserByUsername(username) != nullptr) {
        cout << "Error: Username '" << username << "' already exists.\n";
        return nullptr;
    }

    User* u = new User(explicitId, username, password);
    users.push_back(u);
    usernameToUserId[username] = explicitId;

    if (explicitId >= nextUserId) {
        nextUserId = explicitId + 1;
    }

    return u;
}

// ── Lookup ────────────────────────────────────────────────────────────────────

User* UserManager::findUserById(int id) const {
    for (User* u : users) {
        if (u && u->getUserId() == id) {
            return u;
        }
    }
    return nullptr;
}

User* UserManager::findUserByUsername(const string& username) const {
    auto it = usernameToUserId.find(username);
    if (it == usernameToUserId.end()) {
        return nullptr;
    }
    return findUserById(it->second);
}

const vector<User*>& UserManager::getAllUsers() const {
    return users;
}

// ── Deletion ──────────────────────────────────────────────────────────────────

bool UserManager::deleteUserById(int id) {
    for (auto it = users.begin(); it != users.end(); ++it) {
        User* u = *it;
        if (u && u->getUserId() == id) {
            string username = u->getUsername();
            delete u;
            users.erase(it);
            usernameToUserId.erase(username);
            return true;
        }
    }
    return false;
}

bool UserManager::deleteUserByUsername(const string& username) {
    User* u = findUserByUsername(username);
    if (u) {
        return deleteUserById(u->getUserId());
    }
    return false;
}

// ── Display ───────────────────────────────────────────────────────────────────

void UserManager::listAllUsers() const {
    cout << "\n╔═══════════ Registered Users ═══════════╗\n";
    if (users.empty()) {
        cout << "║  No users registered yet.              ║\n";
    } else {
        for (const User* u : users) {
            if (u) {
                cout << "║  ID: " << u->getUserId() 
                     << " | " << u->getUsername() << "\n";
            }
        }
    }
    cout << "╚════════════════════════════════════════╝\n";
}

// ── Persistence ───────────────────────────────────────────────────────────────

void UserManager::saveToFile(const string& filename) const {
    ofstream out(filename);
    if (!out.is_open()) {
        cout << "Error: Could not open '" << filename << "' for writing.\n";
        return;
    }

    int saved = 0;
    for (const User* u : users) {
        if (!u) continue;

        out << u->getUserId() << '|'
            << u->getUsername() << '|'
            << u->getPassword() << '|';
        
        // Save friend IDs
        const vector<int>& friends = u->getFriendIds();
        for (size_t i = 0; i < friends.size(); i++) {
            out << friends[i];
            if (i < friends.size() - 1) out << ',';
        }
        out << '\n';
        ++saved;
    }

    cout << saved << " user(s) saved to '" << filename << "'.\n";
}

void UserManager::loadFromFile(const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        cout << "No user file found at '" << filename << "'. Starting fresh.\n";
        return;
    }

    clear();

    string line;
    int loaded = 0;

    while (getline(in, line)) {
        if (line.empty()) continue;

        istringstream ss(line);
        string idStr, username, password, friendsStr;

        if (!getline(ss, idStr, '|') ||
            !getline(ss, username, '|') ||
            !getline(ss, password, '|')) {
            cout << "Warning: Skipping malformed user line.\n";
            continue;
        }
            getline(ss, friendsStr);
        try {
            int uid = stoi(idStr);
            User* u = new User(uid, username, password);
            
            // Load friends
            if (!friendsStr.empty()) {
                istringstream friendsStream(friendsStr);
                string friendIdStr;
                while (getline(friendsStream, friendIdStr, ',')) {
                    if (!friendIdStr.empty()) {
                        u->addFriend(stoi(friendIdStr));
                    }
                }
            }
            
            users.push_back(u);
            usernameToUserId[username] = uid;

            if (uid >= nextUserId) {
                nextUserId = uid + 1;
            }
            ++loaded;
        }
        catch (...) {
            cout << "Warning: Invalid user ID in line.\n";
        }
    }

    cout << loaded << " user(s) loaded from '" << filename << "'.\n";
}

// ── Stats ─────────────────────────────────────────────────────────────────────

size_t UserManager::getUserCount() const {
    return users.size();
}

int UserManager::getNextAvailableId() const {
    return nextUserId;
}