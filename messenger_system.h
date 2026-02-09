#ifndef MESSENGER_SYSTEM_H
#define MESSENGER_SYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// Enum for message status
enum class MessageStatus {
    SENT,
    DELIVERED,
    READ
};

// Forward declarations
class Message;
class Conversation;
class GroupChat;

// ============================================================================
// MESSAGE CLASS
// ============================================================================
class Message {
private:
    string messageId;
    string senderId;
    string content;
    time_t timestamp;
    vector<string> likes;  // List of user IDs who liked the message
    MessageStatus status;

public:
    // Constructor
    Message(const string& msgId, const string& sender, const string& cont)
        : messageId(msgId), senderId(sender), content(cont), 
          timestamp(time(nullptr)), status(MessageStatus::SENT) {}

    // Getters
    string getMessageId() const { return messageId; }
    string getSenderId() const { return senderId; }
    string getContent() const { return content; }
    time_t getTimestamp() const { return timestamp; }
    vector<string> getLikes() const { return likes; }
    MessageStatus getStatus() const { return status; }

    // Setters
    void setStatus(MessageStatus s) { status = s; }
    void setContent(const string& cont) { content = cont; }
    void setTimestamp(time_t t) { timestamp = t; }

    // Like functionality
    bool addLike(const string& userId) {
        if (find(likes.begin(), likes.end(), userId) == likes.end()) {
            likes.push_back(userId);
            return true;
        }
        return false;
    }

    bool removeLike(const string& userId) {
        auto it = find(likes.begin(), likes.end(), userId);
        if (it != likes.end()) {
            likes.erase(it);
            return true;
        }
        return false;
    }

    int getLikeCount() const {
        return likes.size();
    }

    // Display
    void display() const {
        cout << "From: " << senderId << endl;
        cout << "Message: " << content << endl;
        cout << "Likes: " << likes.size() << endl;
        cout << "Time: " << ctime(&timestamp);
    }

    // Serialization
    string toCSV() const {
        stringstream ss;
        ss << messageId << "," << senderId << "," << content << "," 
           << timestamp << "," << static_cast<int>(status) << ",";
        
        // Add likes (separated by semicolons)
        for (size_t i = 0; i < likes.size(); i++) {
            ss << likes[i];
            if (i < likes.size() - 1) ss << ";";
        }
        return ss.str();
    }

    static Message fromCSV(const string& csvLine) {
        stringstream ss(csvLine);
        string msgId, sender, cont, likesStr;
        time_t ts;
        int statusInt;
        
        getline(ss, msgId, ',');
        getline(ss, sender, ',');
        getline(ss, cont, ',');
        ss >> ts;
        ss.ignore();
        ss >> statusInt;
        ss.ignore();
        getline(ss, likesStr);

        Message msg(msgId, sender, cont);
        msg.timestamp = ts;
        msg.status = static_cast<MessageStatus>(statusInt);

        // Parse likes
        if (!likesStr.empty()) {
            stringstream likeSS(likesStr);
            string userId;
            while (getline(likeSS, userId, ';')) {
                if (!userId.empty()) {
                    msg.likes.push_back(userId);
                }
            }
        }

        return msg;
    }
};

// ============================================================================
// CONVERSATION CLASS (One-on-one chat)
// ============================================================================
class Conversation {
private:
    string conversationId;
    vector<string> participantIds;  // Exactly 2 participants
    vector<shared_ptr<Message>> messages;
    time_t createdAt;

public:
    // Constructor
    Conversation(const string& convId, const string& user1, const string& user2)
        : conversationId(convId), createdAt(time(nullptr)) {
        participantIds.push_back(user1);
        participantIds.push_back(user2);
        sort(participantIds.begin(), participantIds.end());
    }

    // Getters
    string getConversationId() const { return conversationId; }
    vector<string> getParticipantIds() const { return participantIds; }
    vector<shared_ptr<Message>> getMessages() const { return messages; }
    time_t getCreatedAt() const { return createdAt; }

    // Check if user is participant
    bool isParticipant(const string& userId) const {
        return find(participantIds.begin(), participantIds.end(), userId) != participantIds.end();
    }

    // Add message
    bool addMessage(shared_ptr<Message> message) {
        if (!isParticipant(message->getSenderId())) {
            return false;
        }
        messages.push_back(message);
        return true;
    }

    // Get recent messages
    vector<shared_ptr<Message>> getRecentMessages(int limit = -1) const {
        if (limit < 0 || limit > static_cast<int>(messages.size())) {
            return messages;
        }
        
        vector<shared_ptr<Message>> recent;
        int start = messages.size() - limit;
        for (size_t i = start; i < messages.size(); i++) {
            recent.push_back(messages[i]);
        }
        return recent;
    }

    // Find message by ID
    shared_ptr<Message> findMessage(const string& messageId) {
        for (auto& msg : messages) {
            if (msg->getMessageId() == messageId) {
                return msg;
            }
        }
        return nullptr;
    }

    // Display conversation
    void display() const {
        cout << "\n=== Conversation: " << conversationId << " ===" << endl;
        cout << "Participants: " << participantIds[0] << " <-> " << participantIds[1] << endl;
        cout << "Messages: " << messages.size() << endl;
        cout << "Created: " << ctime(&createdAt);
        
        for (const auto& msg : messages) {
            cout << "\n---" << endl;
            msg->display();
        }
    }

    // Get message count
    int getMessageCount() const {
        return messages.size();
    }
};

// ============================================================================
// GROUP CHAT CLASS
// ============================================================================
class GroupChat {
private:
    string groupId;
    string groupName;
    string adminId;
    vector<string> participantIds;
    vector<shared_ptr<Message>> messages;
    time_t createdAt;

public:
    // Constructor
    GroupChat(const string& grpId, const string& name, const string& admin)
        : groupId(grpId), groupName(name), adminId(admin), createdAt(time(nullptr)) {
        participantIds.push_back(admin);
    }

    // Getters
    string getGroupId() const { return groupId; }
    string getGroupName() const { return groupName; }
    string getAdminId() const { return adminId; }
    vector<string> getParticipantIds() const { return participantIds; }
    vector<shared_ptr<Message>> getMessages() const { return messages; }
    time_t getCreatedAt() const { return createdAt; }

    // Setters
    void setGroupName(const string& name) { groupName = name; }

    // Check if user is participant
    bool isParticipant(const string& userId) const {
        return find(participantIds.begin(), participantIds.end(), userId) != participantIds.end();
    }

    // Check if user is admin
    bool isAdmin(const string& userId) const {
        return userId == adminId;
    }

    // Add participant (only admin can add)
    bool addParticipant(const string& userId, const string& addedBy) {
        if (!isAdmin(addedBy)) {
            return false;
        }
        if (!isParticipant(userId)) {
            participantIds.push_back(userId);
            return true;
        }
        return false;
    }

    // Remove participant (only admin can remove, cannot remove admin)
    bool removeParticipant(const string& userId, const string& removedBy) {
        if (!isAdmin(removedBy) || userId == adminId) {
            return false;
        }
        auto it = find(participantIds.begin(), participantIds.end(), userId);
        if (it != participantIds.end()) {
            participantIds.erase(it);
            return true;
        }
        return false;
    }

    // Add message
    bool addMessage(shared_ptr<Message> message) {
        if (!isParticipant(message->getSenderId())) {
            return false;
        }
        messages.push_back(message);
        return true;
    }

    // Get recent messages
    vector<shared_ptr<Message>> getRecentMessages(int limit = -1) const {
        if (limit < 0 || limit > static_cast<int>(messages.size())) {
            return messages;
        }
        
        vector<shared_ptr<Message>> recent;
        int start = messages.size() - limit;
        for (size_t i = start; i < messages.size(); i++) {
            recent.push_back(messages[i]);
        }
        return recent;
    }

    // Find message by ID
    shared_ptr<Message> findMessage(const string& messageId) {
        for (auto& msg : messages) {
            if (msg->getMessageId() == messageId) {
                return msg;
            }
        }
        return nullptr;
    }

    // Display group
    void display() const {
        cout << "\n=== Group: " << groupName << " ===" << endl;
        cout << "Group ID: " << groupId << endl;
        cout << "Admin: " << adminId << endl;
        cout << "Participants (" << participantIds.size() << "): ";
        for (size_t i = 0; i < participantIds.size(); i++) {
            cout << participantIds[i];
            if (i < participantIds.size() - 1) cout << ", ";
        }
        cout << endl;
        cout << "Messages: " << messages.size() << endl;
        cout << "Created: " << ctime(&createdAt);
        
        for (const auto& msg : messages) {
            cout << "\n---" << endl;
            msg->display();
        }
    }

    // Get message count
    int getMessageCount() const {
        return messages.size();
    }

    // Get participant count
    int getParticipantCount() const {
        return participantIds.size();
    }
};

#endif // MESSENGER_SYSTEM_H
