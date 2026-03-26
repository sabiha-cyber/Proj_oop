#ifndef MESSENGER_MANAGER_H
#define MESSENGER_MANAGER_H

#include "messenger_system.h"
#include "User.h"

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>

using namespace std;

class MessengerManager {
private:
    // Uses real User pointers from UserManager — no ownership
    map<string, User*> users;  // userId string -> User*

    map<string, shared_ptr<Conversation>> conversations;
    map<string, shared_ptr<GroupChat>>    groups;

    string conversationsFile;
    string groupsFile;

    int messageCounter = 0;
    int groupCounter   = 0;

    string currentUserId;
    bool   isLoggedIn = false;

    // ── Helpers ───────────────────────────────────────────────────────────────

    string generateMessageId() {
        return "msg_" + to_string(++messageCounter) + "_" + to_string(time(nullptr));
    }

    string generateGroupId() {
        return "group_" + to_string(++groupCounter) + "_" + to_string(time(nullptr));
    }

    string generateConversationId(const string& u1, const string& u2) const {
        string a = u1 < u2 ? u1 : u2;
        string b = u1 < u2 ? u2 : u1;
        return "conv_" + a + "_" + b;
    }

    bool userExists(const string& userId) const {
        return users.find(userId) != users.end();
    }

    // Polymorphic helper — searches either map via the Chat* base pointer
    shared_ptr<Message> findMessage(const string& messageId,
                                    const string& chatId, bool isGroup) {
        if (isGroup) {
            auto it = groups.find(chatId);
            if (it != groups.end()) return it->second->findMessage(messageId);
        } else {
            auto it = conversations.find(chatId);
            if (it != conversations.end()) return it->second->findMessage(messageId);
        }
        return nullptr;
    }

public:
    // ── Constructor ───────────────────────────────────────────────────────────

    explicit MessengerManager(
        const string& conversationsDB = "conversations.csv",
        const string& groupsDB        = "groups.csv"
    ) : conversationsFile(conversationsDB), groupsFile(groupsDB) {}

    // ── User Registration (bridge from UserManager) ───────────────────────────

    void addUser(User* user) {
        if (user)
            users[to_string(user->getUserId())] = user;
    }

    // ── Session Management ────────────────────────────────────────────────────

    bool login(const string& userId) {
        if (!userExists(userId)) {
            cout << "Error: User not found.\n";
            return false;
        }
        currentUserId = userId;
        isLoggedIn    = true;
        cout << "Messenger logged in as: " << users[userId]->getUsername() << "\n";
        return true;
    }

    void logout() {
        saveDatabase();
        currentUserId = "";
        isLoggedIn    = false;
    }

    bool checkLoggedIn() const {
        if (!isLoggedIn) { cout << "Error: Not logged in.\n"; return false; }
        return true;
    }

    string getCurrentUserId()   const { return currentUserId; }
    bool   isUserLoggedIn()     const { return isLoggedIn;    }

    string getCurrentUsername() const {
        if (isLoggedIn && userExists(currentUserId))
            return users.at(currentUserId)->getUsername();
        return "";
    }

    string getUsername(const string& userId) const {
        auto it = users.find(userId);
        return (it != users.end()) ? it->second->getUsername() : "Unknown";
    }

    vector<pair<string, string>> getAllUsers() const {
        vector<pair<string, string>> result;
        for (const auto& p : users)
            result.push_back({p.first, p.second->getUsername()});
        return result;
    }

    void displayAllUsers() const {
        cout << "\n--- All Users ---\n";
        for (const auto& p : users)
            cout << "  ID: " << p.first << " | " << p.second->getUsername() << "\n";
        cout << "----------------\n";
    }

    // ── One-on-One Messaging ──────────────────────────────────────────────────

    shared_ptr<Message> sendMessage(const string& receiverId, const string& content) {
        if (!checkLoggedIn()) return nullptr;

        try {
            if (!userExists(receiverId))
                throw UserNotFoundException(receiverId);
            if (receiverId == currentUserId)
                throw InvalidContentException("Cannot message yourself.");

            string convId = generateConversationId(currentUserId, receiverId);
            if (conversations.find(convId) == conversations.end())
                conversations[convId] = make_shared<Conversation>(convId, currentUserId, receiverId);

            // InvalidContentException thrown inside Message ctor if content empty
            auto msg = make_shared<Message>(generateMessageId(), currentUserId, content);
            conversations[convId]->addMessage(msg);
            saveConversations();
            cout << "Message sent to " << getUsername(receiverId) << ".\n";
            return msg;

        } catch (const UserNotFoundException& e) {
            cout << "Error: " << e.what() << "\n";
        } catch (const InvalidContentException& e) {
            cout << "Error: " << e.what() << "\n";
        }
        return nullptr;
    }

    shared_ptr<Conversation> getConversation(const string& u1, const string& u2) const {
        string convId = generateConversationId(u1, u2);
        auto it = conversations.find(convId);
        return (it != conversations.end()) ? it->second : nullptr;
    }

    vector<shared_ptr<Conversation>> getMyConversations() const {
        vector<shared_ptr<Conversation>> result;
        for (const auto& p : conversations)
            if (p.second->isParticipant(currentUserId))
                result.push_back(p.second);
        return result;
    }

    // ── Group Messaging ───────────────────────────────────────────────────────

    shared_ptr<GroupChat> createGroup(const string& groupName,
                                      const vector<string>& initialMembers = {}) {
        if (!checkLoggedIn()) return nullptr;

        try {
            if (groupName.empty())
                throw InvalidContentException("Group name cannot be empty.");

            string groupId = generateGroupId();
            auto group = make_shared<GroupChat>(groupId, groupName, currentUserId);

            for (const string& memberId : initialMembers)
                if (userExists(memberId) && memberId != currentUserId)
                    group->addParticipant(memberId, currentUserId);

            groups[groupId] = group;
            saveGroups();
            cout << "Group '" << groupName << "' created (ID: " << groupId << ").\n";
            return group;

        } catch (const InvalidContentException& e) {
            cout << "Error: " << e.what() << "\n";
        }
        return nullptr;
    }

    shared_ptr<Message> sendGroupMessage(const string& groupId, const string& content) {
        if (!checkLoggedIn()) return nullptr;

        try {
            auto it = groups.find(groupId);
            if (it == groups.end())
                throw ChatNotFoundException(groupId);
            if (!it->second->isParticipant(currentUserId))
                throw UnauthorizedException("You are not in this group.");

            // InvalidContentException thrown inside Message ctor if content empty
            auto msg = make_shared<Message>(generateMessageId(), currentUserId, content);
            it->second->addMessage(msg);
            saveGroups();
            cout << "Group message sent.\n";
            return msg;

        } catch (const ChatNotFoundException& e) {
            cout << "Error: " << e.what() << "\n";
        } catch (const UnauthorizedException& e) {
            cout << "Error: " << e.what() << "\n";
        } catch (const InvalidContentException& e) {
            cout << "Error: " << e.what() << "\n";
        }
        return nullptr;
    }

    shared_ptr<GroupChat> getGroup(const string& groupId) const {
        auto it = groups.find(groupId);
        return (it != groups.end()) ? it->second : nullptr;
    }

    vector<shared_ptr<GroupChat>> getMyGroups() const {
        vector<shared_ptr<GroupChat>> result;
        for (const auto& p : groups)
            if (p.second->isParticipant(currentUserId))
                result.push_back(p.second);
        return result;
    }

    // ── Group Member Management (wires up previously dead removeParticipant) ──

    bool addMemberToGroup(const string& groupId, const string& userId) {
        if (!checkLoggedIn()) return false;
        try {
            auto it = groups.find(groupId);
            if (it == groups.end()) throw ChatNotFoundException(groupId);
            if (!it->second->isAdmin(currentUserId))
                throw UnauthorizedException("Only the admin can add members.");
            if (!userExists(userId)) throw UserNotFoundException(userId);

            bool ok = it->second->addParticipant(userId, currentUserId);
            if (ok) { saveGroups(); cout << getUsername(userId) << " added to group.\n"; }
            else      cout << "User is already a member.\n";
            return ok;

        } catch (const SocialException& e) {
            cout << "Error: " << e.what() << "\n";
        }
        return false;
    }

    bool removeMemberFromGroup(const string& groupId, const string& userId) {
        if (!checkLoggedIn()) return false;
        try {
            auto it = groups.find(groupId);
            if (it == groups.end()) throw ChatNotFoundException(groupId);
            if (!it->second->isAdmin(currentUserId))
                throw UnauthorizedException("Only the admin can remove members.");

            bool ok = it->second->removeParticipant(userId, currentUserId);
            if (ok) { saveGroups(); cout << getUsername(userId) << " removed from group.\n"; }
            else      cout << "Could not remove member (not found or is admin).\n";
            return ok;

        } catch (const SocialException& e) {
            cout << "Error: " << e.what() << "\n";
        }
        return false;
    }

    // ── Likes ─────────────────────────────────────────────────────────────────

    bool likeMessage(const string& messageId, const string& chatId, bool isGroup = false) {
        if (!checkLoggedIn()) return false;
        shared_ptr<Message> msg = findMessage(messageId, chatId, isGroup);
        if (!msg) { cout << "Message not found.\n"; return false; }
        bool ok = msg->addLike(currentUserId);
        cout << (ok ? "Message liked.\n" : "Already liked.\n");
        if (ok) isGroup ? saveGroups() : saveConversations();
        return ok;
    }

    bool unlikeMessage(const string& messageId, const string& chatId, bool isGroup = false) {
        if (!checkLoggedIn()) return false;
        shared_ptr<Message> msg = findMessage(messageId, chatId, isGroup);
        if (!msg) { cout << "Message not found.\n"; return false; }
        bool ok = msg->removeLike(currentUserId);
        cout << (ok ? "Like removed.\n" : "You haven't liked this.\n");
        if (ok) isGroup ? saveGroups() : saveConversations();
        return ok;
    }

    // ── Persistence ───────────────────────────────────────────────────────────

    void loadDatabase() {
        loadConversations();
        loadGroups();
    }

    void saveDatabase() {
        saveConversations();
        saveGroups();
    }

private:
    void saveConversations() {
        try {
            ofstream file(conversationsFile);
            if (!file.is_open())
                throw FileIOException("Cannot open file: " + conversationsFile);

            file << "conversationId,participant1,participant2,messageData\n";
            for (const auto& p : conversations) {
                auto conv  = p.second;
                auto parts = conv->getParticipantIds();
                for (const auto& msg : conv->getMessages())
                    file << conv->getConversationId() << ","
                         << parts[0] << "," << parts[1] << ","
                         << msg->toCSV() << "\n";
            }
        } catch (const FileIOException& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }

    void loadConversations() {
        try {
            ifstream file(conversationsFile);
            if (!file.is_open()) return;   // first run — file doesn't exist yet

            string line;
            getline(file, line); // skip header
            while (getline(file, line)) {
                if (line.empty()) continue;
                stringstream ss(line);
                string convId, p1, p2, msgData;
                getline(ss, convId,   ',');
                getline(ss, p1,       ',');
                getline(ss, p2,       ',');
                getline(ss, msgData);

                if (conversations.find(convId) == conversations.end())
                    conversations[convId] = make_shared<Conversation>(convId, p1, p2);

                if (!msgData.empty()) {
                    auto msg = make_shared<Message>(Message::fromCSV(msgData));
                    conversations[convId]->addMessage(msg);
                }
            }
        } catch (const InvalidContentException& e) {
            cout << "Warning: skipped malformed message during load — " << e.what() << "\n";
        }
    }

    void saveGroups() {
        try {
            ofstream file(groupsFile);
            if (!file.is_open())
                throw FileIOException("Cannot open file: " + groupsFile);

            file << "groupId,groupName,adminId,participants,messageData\n";
            for (const auto& p : groups) {
                auto  group = p.second;
                auto  parts = group->getParticipantIds();
                stringstream pSS;
                for (size_t i = 0; i < parts.size(); i++) {
                    pSS << parts[i];
                    if (i < parts.size() - 1) pSS << ";";
                }
                for (const auto& msg : group->getMessages())
                    file << group->getGroupId()   << ","
                         << group->getGroupName() << ","
                         << group->getAdminId()   << ","
                         << pSS.str()             << ","
                         << msg->toCSV()          << "\n";
            }
        } catch (const FileIOException& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }

    void loadGroups() {
        try {
            ifstream file(groupsFile);
            if (!file.is_open()) return;

            string line;
            getline(file, line); // skip header
            while (getline(file, line)) {
                if (line.empty()) continue;
                stringstream ss(line);
                string groupId, groupName, adminId, partsStr, msgData;
                getline(ss, groupId,   ',');
                getline(ss, groupName, ',');
                getline(ss, adminId,   ',');
                getline(ss, partsStr,  ',');
                getline(ss, msgData);

                if (groups.find(groupId) == groups.end()) {
                    groups[groupId] = make_shared<GroupChat>(groupId, groupName, adminId);
                    stringstream pSS(partsStr);
                    string uid;
                    while (getline(pSS, uid, ';'))
                        if (!uid.empty() && uid != adminId)
                            groups[groupId]->addParticipant(uid, adminId);
                }

                if (!msgData.empty()) {
                    auto msg = make_shared<Message>(Message::fromCSV(msgData));
                    groups[groupId]->addMessage(msg);
                }
            }
        } catch (const InvalidContentException& e) {
            cout << "Warning: skipped malformed message during load — " << e.what() << "\n";
        }
    }
};

#endif // MESSENGER_MANAGER_H
