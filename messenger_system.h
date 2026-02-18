#ifndef MESSENGER_SYSTEM_H
#define MESSENGER_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>    // better than map for user lookups if needed
#include <memory>           // shared_ptr
#include <ctime>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>          // for better time formatting (optional)

// Forward declarations
class User;                 // If integrating with your User class (recommended)

// ============================================================================
// Helper functions for CSV escaping (simple version)
// ============================================================================
namespace csv_util {
    inline std::string escape(const std::string& field) {
        if (field.find(',') == std::string::npos &&
            field.find('"') == std::string::npos &&
            field.find('\n') == std::string::npos) {
            return field;
        }
        std::string result = "\"";
        for (char c : field) {
            if (c == '"') result += "\"\"";
            else result += c;
        }
        result += "\"";
        return result;
    }

    inline std::string unescape(const std::string& field) {
        if (field.size() >= 2 && field.front() == '"' && field.back() == '"') {
            std::string result;
            for (size_t i = 1; i < field.size() - 1; ++i) {
                if (field[i] == '"' && i + 1 < field.size() - 1 && field[i+1] == '"') {
                    result += '"';
                    ++i;
                } else {
                    result += field[i];
                }
            }
            return result;
        }
        return field;
    }
}

// ============================================================================
// Enum for message status
// ============================================================================
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
    std::string messageId;
    std::string senderId;
    std::string content;
    std::time_t timestamp;
    std::vector<std::string> likes;
    MessageStatus status;

public:
    Message(const std::string& msgId, const std::string& sender, const std::string& cont)
        : messageId(msgId), senderId(sender), content(cont),
          timestamp(std::time(nullptr)), status(MessageStatus::SENT) {}

    // Getters
    std::string getMessageId() const { return messageId; }
    std::string getSenderId() const { return senderId; }
    std::string getContent() const { return content; }
    std::time_t getTimestamp() const { return timestamp; }
    const std::vector<std::string>& getLikes() const { return likes; }
    MessageStatus getStatus() const { return status; }
    int getLikeCount() const { return static_cast<int>(likes.size()); }

    // Setters
    void setStatus(MessageStatus s) { status = s; }
    void setContent(const std::string& cont) { content = cont; }

    // Like functionality
    bool addLike(const std::string& userId) {
        if (std::find(likes.begin(), likes.end(), userId) == likes.end()) {
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

    bool removeLike(const std::string& userId) {
        auto it = std::find(likes.begin(), likes.end(), userId);
    bool removeLike(const string& userId) {
        auto it = find(likes.begin(), likes.end(), userId);
        if (it != likes.end()) {
            likes.erase(it);
            return true;
        }
        return false;
    }

    // Display (simple console version)
    void display() const {
        std::cout << "From: " << senderId << "  |  "
                  << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S") << "\n"
                  << "  " << content << "\n"
                  << "  Likes: " << likes.size() << "\n";
    }

    // CSV serialization (improved escaping)
    std::string toCSV() const {
        std::ostringstream ss;
        ss << csv_util::escape(messageId) << ","
           << csv_util::escape(senderId) << ","
           << csv_util::escape(content) << ","
           << timestamp << ","
           << static_cast<int>(status) << ",";

        for (size_t i = 0; i < likes.size(); ++i) {
            ss << csv_util::escape(likes[i]);
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

    static Message fromCSV(const std::string& csvLine) {
        std::istringstream ss(csvLine);
        std::string token;
        std::vector<std::string> fields;

        while (std::getline(ss, token, ',')) {
            fields.push_back(csv_util::unescape(token));
        }

        if (fields.size() < 5) {
            throw std::invalid_argument("Invalid CSV line for Message");
        }

        Message msg(fields[0], fields[1], fields[2]);
        msg.timestamp = std::stoll(fields[3]);
        msg.status = static_cast<MessageStatus>(std::stoi(fields[4]));

        // Parse likes if present
        if (fields.size() >= 6 && !fields[5].empty()) {
            std::istringstream likeSS(fields[5]);
            std::string userId;
            while (std::getline(likeSS, userId, ';')) {
                if (!userId.empty()) {
                    msg.likes.push_back(csv_util::unescape(userId));
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
// CONVERSATION CLASS (1-on-1)
// ============================================================================
class Conversation {
private:
    std::string conversationId;
    std::vector<std::string> participantIds;          // exactly 2
    std::vector<std::shared_ptr<Message>> messages;
    std::time_t createdAt;

public:
    Conversation(const std::string& convId, const std::string& u1, const std::string& u2)
        : conversationId(convId), createdAt(std::time(nullptr)) {
        participantIds = {u1, u2};
        std::sort(participantIds.begin(), participantIds.end());
    }

    std::string getConversationId() const { return conversationId; }
    const std::vector<std::string>& getParticipantIds() const { return participantIds; }
    const std::vector<std::shared_ptr<Message>>& getMessages() const { return messages; }
    std::time_t getCreatedAt() const { return createdAt; }

    bool isParticipant(const std::string& userId) const {
        return std::find(participantIds.begin(), participantIds.end(), userId) != participantIds.end();
    }

    bool addMessage(std::shared_ptr<Message> message) {
        if (!isParticipant(message->getSenderId())) return false;
        messages.push_back(std::move(message));
        return true;
    }

    std::shared_ptr<Message> findMessage(const std::string& messageId) const {
        for (const auto& msg : messages) {
            if (msg && msg->getMessageId() == messageId) {
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

    int getMessageCount() const { return static_cast<int>(messages.size()); }

    void display() const;  // can be implemented in .cpp if desired
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
    std::string groupId;
    std::string groupName;
    std::string adminId;
    std::vector<std::string> participantIds;
    std::vector<std::shared_ptr<Message>> messages;
    std::time_t createdAt;

public:
    GroupChat(const std::string& grpId, const std::string& name, const std::string& admin)
        : groupId(grpId), groupName(name), adminId(admin), createdAt(std::time(nullptr)) {
        participantIds.push_back(admin);
    }

    std::string getGroupId() const { return groupId; }
    std::string getGroupName() const { return groupName; }
    std::string getAdminId() const { return adminId; }
    const std::vector<std::string>& getParticipantIds() const { return participantIds; }
    const std::vector<std::shared_ptr<Message>>& getMessages() const { return messages; }

    bool isParticipant(const std::string& userId) const {
        return std::find(participantIds.begin(), participantIds.end(), userId) != participantIds.end();
    }

    bool isAdmin(const std::string& userId) const { return userId == adminId; }

    bool addParticipant(const std::string& userId, const std::string& addedBy) {
        if (!isAdmin(addedBy) || isParticipant(userId)) return false;
        participantIds.push_back(userId);
        return true;
    }

    bool removeParticipant(const std::string& userId, const std::string& removedBy) {
        if (!isAdmin(removedBy) || userId == adminId) return false;
        auto it = std::find(participantIds.begin(), participantIds.end(), userId);
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

    bool addMessage(std::shared_ptr<Message> message) {
        if (!isParticipant(message->getSenderId())) return false;
        messages.push_back(std::move(message));
        return true;
    }

    std::shared_ptr<Message> findMessage(const std::string& messageId) const {
        for (const auto& msg : messages) {
            if (msg && msg->getMessageId() == messageId) return msg;
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

    int getMessageCount() const { return static_cast<int>(messages.size()); }
    int getParticipantCount() const { return static_cast<int>(participantIds.size()); }

    void display() const;  // can be moved to .cpp
};

#endif 
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
