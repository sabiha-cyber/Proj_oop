#ifndef MESSENGER_MANAGER_H
#define MESSENGER_MANAGER_H

#include "messenger_system.h"
#include "User.h"               // Assuming your User class header

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <utility>              // std::move, std::pair

// ============================================================================
// MESSENGER MANAGER CLASS
// Central class for messenger operations + file-based persistence
// ============================================================================
class MessengerManager {
private:
    // ────────────────────────────────────────────────
    // Storage ─ using real User objects now
    std::unordered_map<std::string, std::shared_ptr<User>> users;

    std::unordered_map<std::string, std::shared_ptr<Conversation>> conversations;
    std::unordered_map<std::string, std::shared_ptr<GroupChat>>    groups;

    // File paths
    std::string usersFile;
    std::string conversationsFile;
    std::string groupsFile;

    // Session
    std::string currentUserId;
    bool        isLoggedIn = false;

    // Helpers ─ ID generation (static counters = simpler, no need to persist)
    static std::string generateMessageId() {
        static int counter = 0;
        return "msg_" + std::to_string(++counter) + "_" + std::to_string(std::time(nullptr));
    }

    static std::string generateGroupId() {
        static int counter = 0;
        return "group_" + std::to_string(++counter) + "_" + std::to_string(std::time(nullptr));
    }

    std::string generateConversationId(const std::string& u1, const std::string& u2) const {
        std::string a = u1 < u2 ? u1 : u2;
        std::string b = u1 < u2 ? u2 : u1;
        return "conv_" + a + "_" + b;
    }

    bool userExists(const std::string& userId) const {
        return users.find(userId) != users.end();
    }

public:
    // Constructor
    explicit MessengerManager(
        const std::string& usersDB         = "users.csv",
        const std::string& conversationsDB = "conversations.csv",
        const std::string& groupsDB        = "groups.csv"
    );

    // ────────────────────────────────────────────────
    // Session Management
    // ────────────────────────────────────────────────
    bool login(const std::string& userId);
    void logout();
    bool checkLoggedIn() const;
    std::string getCurrentUserId() const { return currentUserId; }
    std::string getCurrentUsername() const;
    bool isUserLoggedIn() const { return isLoggedIn; }

    // ────────────────────────────────────────────────
    // User Management
    // ────────────────────────────────────────────────
    bool registerUser(const std::string& userId,
                      const std::string& username,
                      const std::string& password = "");

    bool isValidUser(const std::string& userId) const { return userExists(userId); }
    std::shared_ptr<User>       getUser(const std::string& userId) const;
    std::string                 getUsername(const std::string& userId) const;
    void                        displayAllUsers() const;

    // ────────────────────────────────────────────────
    // One-on-One Messaging
    // ────────────────────────────────────────────────
    std::shared_ptr<Message> sendMessage(const std::string& receiverId, const std::string& content);
    std::shared_ptr<Conversation> getConversation(const std::string& user1, const std::string& user2) const;
    std::vector<std::shared_ptr<Conversation>> getMyConversations() const;
    std::vector<std::shared_ptr<Conversation>> getUserConversations(const std::string& userId) const;

    // ────────────────────────────────────────────────
    // Group Messaging
    // ────────────────────────────────────────────────
    std::shared_ptr<GroupChat> createGroup(
        const std::string& groupName,
        const std::vector<std::string>& initialMembers = {});

    std::shared_ptr<Message> sendGroupMessage(const std::string& groupId, const std::string& content);
    std::shared_ptr<GroupChat> getGroup(const std::string& groupId) const;
    std::vector<std::shared_ptr<GroupChat>> getMyGroups() const;
    std::vector<std::shared_ptr<GroupChat>> getUserGroups(const std::string& userId) const;

    // ────────────────────────────────────────────────
    // Likes / Reactions
    // ────────────────────────────────────────────────
    bool likeMessage(const std::string& messageId, const std::string& chatId, bool isGroup = false);
    bool unlikeMessage(const std::string& messageId, const std::string& chatId, bool isGroup = false);

    // ────────────────────────────────────────────────
    // Persistence
    // ────────────────────────────────────────────────
    void loadDatabase();
    void saveDatabase();

private:
    void loadUsers();
    void saveUsers();

    void loadConversations();
    void saveConversations();

    void loadGroups();
    void saveGroups();

public:
    void displayStatistics() const;
};

#endif // MESSENGER_MANAGER_H