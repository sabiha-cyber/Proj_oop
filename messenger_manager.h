#ifndef MESSENGER_MANAGER_H
#define MESSENGER_MANAGER_H

#include "messenger_system.h"
#include <unordered_map>
#include <sstream>
#include <iomanip>

// ============================================================================
// MESSENGER MANAGER CLASS
// Handles all messenger operations and data persistence
// ============================================================================
class MessengerManager {
private:
    // In-memory storage
    map<string, string> users;  // userId -> username (storing minimal user data)
    map<string, shared_ptr<Conversation>> conversations;
    map<string, shared_ptr<GroupChat>> groups;

    // Database file paths
    string usersFile;
    string conversationsFile;
    string groupsFile;

    // Counters for ID generation
    int messageCounter;
    int conversationCounter;
    int groupCounter;

    // Session management
    string currentUserId;
    bool isLoggedIn;

    // Helper functions
    string generateMessageId() {
        return "msg_" + to_string(++messageCounter) + "_" + to_string(time(nullptr));
    }

    string generateConversationId(const string& user1, const string& user2) {
        vector<string> ids = {user1, user2};
        sort(ids.begin(), ids.end());
        return "conv_" + ids[0] + "_" + ids[1];
    }

    string generateGroupId() {
        return "group_" + to_string(++groupCounter) + "_" + to_string(time(nullptr));
    }

    // Validation
    bool userExists(const string& userId) const {
        return users.find(userId) != users.end();
    }

public:
    // Constructor
    MessengerManager(const string& usersDB = "users.csv",
                    const string& conversationsDB = "conversations.csv",
                    const string& groupsDB = "groups.csv")
        : usersFile(usersDB), conversationsFile(conversationsDB), 
          groupsFile(groupsDB), messageCounter(0), conversationCounter(0), 
          groupCounter(0), currentUserId(""), isLoggedIn(false) {
        loadDatabase();
    }

    // ========================================================================
    // SESSION MANAGEMENT
    // ========================================================================
    bool login(const string& userId) {
        if (!userExists(userId)) {
            cout << "Error: User ID not found!" << endl;
            return false;
        }
        currentUserId = userId;
        isLoggedIn = true;
        cout << "Logged in as: " << users[userId] << " (ID: " << userId << ")" << endl;
        return true;
    }

    void logout() {
        if (isLoggedIn) {
            cout << "Logged out: " << users[currentUserId] << endl;
            currentUserId = "";
            isLoggedIn = false;
        }
    }

    bool checkLoggedIn() const {
        if (!isLoggedIn) {
            cout << "Error: You must be logged in to perform this action!" << endl;
            return false;
        }
        return true;
    }

    string getCurrentUserId() const {
        return currentUserId;
    }

    string getCurrentUsername() const {
        if (isLoggedIn) {
            return users.at(currentUserId);
        }
        return "";
    }

    bool isUserLoggedIn() const {
        return isLoggedIn;
    }

    // ========================================================================
    // USER MANAGEMENT
    // ========================================================================
    bool registerUser(const string& userId, const string& username) {
        if (userExists(userId)) {
            cout << "Error: User ID already exists!" << endl;
            return false;
        }
        users[userId] = username;
        saveUsers();
        cout << "User registered: " << username << " (ID: " << userId << ")" << endl;
        return true;
    }

    bool isValidUser(const string& userId) const {
        return userExists(userId);
    }

    string getUsername(const string& userId) const {
        auto it = users.find(userId);
        if (it != users.end()) {
            return it->second;
        }
        return "";
    }

    vector<pair<string, string>> getAllUsers() const {
        vector<pair<string, string>> allUsers;
        for (const auto& pair : users) {
            allUsers.push_back({pair.first, pair.second});
        }
        return allUsers;
    }

    void displayAllUsers() const {
        cout << "\n=== Registered Users ===" << endl;
        for (const auto& pair : users) {
            cout << "ID: " << pair.first << " | Name: " << pair.second << endl;
        }
    }

    // ========================================================================
    // MESSAGING (One-on-One)
    // ========================================================================
    shared_ptr<Message> sendMessage(const string& receiverId, const string& content) {
        // Check if logged in
        if (!checkLoggedIn()) return nullptr;

        // Validate receiver
        if (!userExists(receiverId)) {
            cout << "Error: Receiver does not exist!" << endl;
            return nullptr;
        }

        if (receiverId == currentUserId) {
            cout << "Error: Cannot send message to yourself!" << endl;
            return nullptr;
        }

        // Get or create conversation
        string convId = generateConversationId(currentUserId, receiverId);
        shared_ptr<Conversation> conv;

        auto it = conversations.find(convId);
        if (it == conversations.end()) {
            conv = make_shared<Conversation>(convId, currentUserId, receiverId);
            conversations[convId] = conv;
        } else {
            conv = it->second;
        }

        // Create and add message
        auto message = make_shared<Message>(generateMessageId(), currentUserId, content);
        conv->addMessage(message);

        // Save to database
        saveConversations();

        cout << "Message sent to " << getUsername(receiverId) << endl;

        return message;
    }

    shared_ptr<Conversation> getConversation(const string& user1, const string& user2) {
        string convId = generateConversationId(user1, user2);
        auto it = conversations.find(convId);
        if (it != conversations.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Get all conversations for logged-in user
    vector<shared_ptr<Conversation>> getMyConversations() {
        if (!checkLoggedIn()) return {};
        
        vector<shared_ptr<Conversation>> userConvs;
        for (const auto& pair : conversations) {
            if (pair.second->isParticipant(currentUserId)) {
                userConvs.push_back(pair.second);
            }
        }
        return userConvs;
    }

    vector<shared_ptr<Conversation>> getUserConversations(const string& userId) {
        vector<shared_ptr<Conversation>> userConvs;
        for (const auto& pair : conversations) {
            if (pair.second->isParticipant(userId)) {
                userConvs.push_back(pair.second);
            }
        }
        return userConvs;
    }

    // ========================================================================
    // GROUP MESSAGING
    // ========================================================================
    shared_ptr<GroupChat> createGroup(const string& groupName,
                                     const vector<string>& participantIds = {}) {
        // Check if logged in
        if (!checkLoggedIn()) return nullptr;

        // Validate participants
        for (const auto& userId : participantIds) {
            if (!userExists(userId)) {
                cout << "Error: Participant " << userId << " does not exist!" << endl;
                return nullptr;
            }
        }

        // Create group (logged-in user is automatically admin)
        auto group = make_shared<GroupChat>(generateGroupId(), groupName, currentUserId);

        // Add participants
        for (const auto& userId : participantIds) {
            if (userId != currentUserId) {
                group->addParticipant(userId, currentUserId);
            }
        }

        groups[group->getGroupId()] = group;
        saveGroups();

        cout << "Group created: " << groupName << " (You are the admin)" << endl;
        cout << "Total members: " << group->getParticipantCount() << endl;

        return group;
    }

    shared_ptr<Message> sendGroupMessage(const string& groupId, const string& content) {
        // Check if logged in
        if (!checkLoggedIn()) return nullptr;

        // Get group
        auto it = groups.find(groupId);
        if (it == groups.end()) {
            cout << "Error: Group does not exist!" << endl;
            return nullptr;
        }

        auto group = it->second;

        // Check if logged-in user is participant
        if (!group->isParticipant(currentUserId)) {
            cout << "Error: You are not a member of this group!" << endl;
            return nullptr;
        }

        // Create and add message
        auto message = make_shared<Message>(generateMessageId(), currentUserId, content);
        group->addMessage(message);

        // Save to database
        saveGroups();

        cout << "Message sent to " << group->getGroupName() << endl;

        return message;
    }

    shared_ptr<GroupChat> getGroup(const string& groupId) {
        auto it = groups.find(groupId);
        if (it != groups.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Get all groups for logged-in user
    vector<shared_ptr<GroupChat>> getMyGroups() {
        if (!checkLoggedIn()) return {};
        
        vector<shared_ptr<GroupChat>> userGroups;
        for (const auto& pair : groups) {
            if (pair.second->isParticipant(currentUserId)) {
                userGroups.push_back(pair.second);
            }
        }
        return userGroups;
    }

    vector<shared_ptr<GroupChat>> getUserGroups(const string& userId) {
        vector<shared_ptr<GroupChat>> userGroups;
        for (const auto& pair : groups) {
            if (pair.second->isParticipant(userId)) {
                userGroups.push_back(pair.second);
            }
        }
        return userGroups;
    }

    // ========================================================================
    // LIKES
    // ========================================================================
    bool likeMessage(const string& messageId, const string& chatId, bool isGroup = false) {
        if (!checkLoggedIn()) return false;

        shared_ptr<Message> message = nullptr;

        if (isGroup) {
            auto group = getGroup(chatId);
            if (group && group->isParticipant(currentUserId)) {
                message = group->findMessage(messageId);
            } else {
                cout << "Error: You are not a member of this group!" << endl;
                return false;
            }
        } else {
            auto it = conversations.find(chatId);
            if (it != conversations.end() && it->second->isParticipant(currentUserId)) {
                message = it->second->findMessage(messageId);
            } else {
                cout << "Error: You are not a participant in this conversation!" << endl;
                return false;
            }
        }

        if (message) {
            bool result = message->addLike(currentUserId);
            if (result) {
                cout << "Message liked!" << endl;
            } else {
                cout << "You already liked this message!" << endl;
            }
            // Save changes
            if (isGroup) {
                saveGroups();
            } else {
                saveConversations();
            }
            return result;
        }

        cout << "Error: Message not found!" << endl;
        return false;
    }

    bool unlikeMessage(const string& messageId, const string& chatId, bool isGroup = false) {
        if (!checkLoggedIn()) return false;

        shared_ptr<Message> message = nullptr;

        if (isGroup) {
            auto group = getGroup(chatId);
            if (group && group->isParticipant(currentUserId)) {
                message = group->findMessage(messageId);
            }
        } else {
            auto it = conversations.find(chatId);
            if (it != conversations.end() && it->second->isParticipant(currentUserId)) {
                message = it->second->findMessage(messageId);
            }
        }

        if (message) {
            bool result = message->removeLike(currentUserId);
            if (result) {
                cout << "Like removed!" << endl;
            } else {
                cout << "You haven't liked this message!" << endl;
            }
            // Save changes
            if (isGroup) {
                saveGroups();
            } else {
                saveConversations();
            }
            return result;
        }

        return false;
    }

    // ========================================================================
    // DATABASE OPERATIONS
    // ========================================================================
    void saveUsers() {
        ofstream file(usersFile);
        if (!file.is_open()) {
            cout << "Error: Could not open users file for writing!" << endl;
            return;
        }

        file << "userId,username\n";
        for (const auto& pair : users) {
            file << pair.first << "," << pair.second << "\n";
        }
        file.close();
    }

    void saveConversations() {
        ofstream file(conversationsFile);
        if (!file.is_open()) {
            cout << "Error: Could not open conversations file for writing!" << endl;
            return;
        }

        file << "conversationId,participant1,participant2,messageData\n";
        for (const auto& pair : conversations) {
            auto conv = pair.second;
            auto participants = conv->getParticipantIds();
            
            for (const auto& msg : conv->getMessages()) {
                file << conv->getConversationId() << ","
                     << participants[0] << ","
                     << participants[1] << ","
                     << msg->toCSV() << "\n";
            }
        }
        file.close();
    }

    void saveGroups() {
        ofstream file(groupsFile);
        if (!file.is_open()) {
            cout << "Error: Could not open groups file for writing!" << endl;
            return;
        }

        file << "groupId,groupName,adminId,participants,messageData\n";
        for (const auto& pair : groups) {
            auto group = pair.second;
            
            // Serialize participants
            stringstream participantsSS;
            auto participants = group->getParticipantIds();
            for (size_t i = 0; i < participants.size(); i++) {
                participantsSS << participants[i];
                if (i < participants.size() - 1) participantsSS << ";";
            }

            for (const auto& msg : group->getMessages()) {
                file << group->getGroupId() << ","
                     << group->getGroupName() << ","
                     << group->getAdminId() << ","
                     << participantsSS.str() << ","
                     << msg->toCSV() << "\n";
            }
        }
        file.close();
    }

    void loadDatabase() {
        loadUsers();
        loadConversations();
        loadGroups();
    }

    void loadUsers() {
        ifstream file(usersFile);
        if (!file.is_open()) {
            return;  // File doesn't exist yet
        }

        string line;
        getline(file, line);  // Skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string userId, username;
            getline(ss, userId, ',');
            getline(ss, username);
            users[userId] = username;
        }
        file.close();
        cout << "Loaded " << users.size() << " users from database" << endl;
    }

    void loadConversations() {
        ifstream file(conversationsFile);
        if (!file.is_open()) {
            return;
        }

        string line;
        getline(file, line);  // Skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string convId, p1, p2, msgData;
            getline(ss, convId, ',');
            getline(ss, p1, ',');
            getline(ss, p2, ',');
            getline(ss, msgData);

            // Get or create conversation
            if (conversations.find(convId) == conversations.end()) {
                conversations[convId] = make_shared<Conversation>(convId, p1, p2);
            }

            // Add message
            auto msg = make_shared<Message>(Message::fromCSV(msgData));
            conversations[convId]->addMessage(msg);
        }
        file.close();
        cout << "Loaded " << conversations.size() << " conversations from database" << endl;
    }

    void loadGroups() {
        ifstream file(groupsFile);
        if (!file.is_open()) {
            return;
        }

        string line;
        getline(file, line);  // Skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string groupId, groupName, adminId, participantsStr, msgData;
            getline(ss, groupId, ',');
            getline(ss, groupName, ',');
            getline(ss, adminId, ',');
            getline(ss, participantsStr, ',');
            getline(ss, msgData);

            // Get or create group
            if (groups.find(groupId) == groups.end()) {
                groups[groupId] = make_shared<GroupChat>(groupId, groupName, adminId);
                
                // Add participants
                stringstream pSS(participantsStr);
                string userId;
                while (getline(pSS, userId, ';')) {
                    if (!userId.empty() && userId != adminId) {
                        groups[groupId]->addParticipant(userId, adminId);
                    }
                }
            }

            // Add message
            auto msg = make_shared<Message>(Message::fromCSV(msgData));
            groups[groupId]->addMessage(msg);
        }
        file.close();
        cout << "Loaded " << groups.size() << " groups from database" << endl;
    }

    // ========================================================================
    // UTILITY FUNCTIONS
    // ========================================================================
    void displayStatistics() const {
        cout << "\n=== Messenger Statistics ===" << endl;
        cout << "Total Users: " << users.size() << endl;
        cout << "Total Conversations: " << conversations.size() << endl;
        cout << "Total Groups: " << groups.size() << endl;
        
        int totalMessages = 0;
        for (const auto& pair : conversations) {
            totalMessages += pair.second->getMessageCount();
        }
        for (const auto& pair : groups) {
            totalMessages += pair.second->getMessageCount();
        }
        cout << "Total Messages: " << totalMessages << endl;
    }
};

#endif // MESSENGER_MANAGER_H
