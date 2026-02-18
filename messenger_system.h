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
enum class MessageStatus {
    SENT,
    DELIVERED,
    READ
};

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
            likes.push_back(userId);
            return true;
        }
        return false;
    }

    bool removeLike(const std::string& userId) {
        auto it = std::find(likes.begin(), likes.end(), userId);
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
                return msg;
            }
        }
        return nullptr;
    }

    int getMessageCount() const { return static_cast<int>(messages.size()); }

    void display() const;  // can be implemented in .cpp if desired
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
        }
        return nullptr;
    }

    int getMessageCount() const { return static_cast<int>(messages.size()); }
    int getParticipantCount() const { return static_cast<int>(participantIds.size()); }

    void display() const;  // can be moved to .cpp
};

#endif 