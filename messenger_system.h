#ifndef MESSENGER_SYSTEM_H
#define MESSENGER_SYSTEM_H

#include "SocialExceptions.h"

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <algorithm>
#include <iostream>
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
class Chat;
class Conversation;
class GroupChat;

// ============================================================================
// TEMPLATE UTILITY
// Search any vector<shared_ptr<T>> by a string key extracted via a lambda.
// Usage:  findByKey<Message>(messages, [](auto m){ return m->getMessageId(); }, id)
// ============================================================================
template <typename T, typename KeyFn>
shared_ptr<T> findByKey(const vector<shared_ptr<T>>& vec,
                        KeyFn keyFn,
                        const string& target) {
    for (const auto& item : vec)
        if (keyFn(item) == target) return item;
    return nullptr;
}

// ============================================================================
// MESSAGE CLASS
// ============================================================================
class Message {
private:
    string         messageId;
    string         senderId;
    string         content;
    time_t         timestamp;
    vector<string> likes;
    MessageStatus  status;

public:
    // ── Constructor ───────────────────────────────────────────────────────────
    Message(const string& msgId, const string& sender, const string& cont)
        : messageId(msgId), senderId(sender), content(cont),
          timestamp(time(nullptr)), status(MessageStatus::SENT) {
        if (cont.empty())
            throw InvalidContentException("Message content cannot be empty.");
    }

    // ── Copy constructor & copy-assignment (explicit, rule of three) ──────────
    Message(const Message& other) = default;
    Message& operator=(const Message& other) = default;

    // ── Getters ───────────────────────────────────────────────────────────────
    string         getMessageId() const { return messageId; }
    string         getSenderId()  const { return senderId;  }
    string         getContent()   const { return content;   }
    time_t         getTimestamp() const { return timestamp; }
    vector<string> getLikes()     const { return likes;     }
    MessageStatus  getStatus()    const { return status;    }

    // ── Setters ───────────────────────────────────────────────────────────────
    void setStatus(MessageStatus s)      { status  = s;    }
    void setContent(const string& cont)  { content = cont; }
    void setTimestamp(time_t t)          { timestamp = t;  }

    // ── Like functionality ────────────────────────────────────────────────────
    bool addLike(const string& userId) {
        if (find(likes.begin(), likes.end(), userId) == likes.end()) {
            likes.push_back(userId);
            return true;
        }
        return false;
    }

    bool removeLike(const string& userId) {
        auto it = find(likes.begin(), likes.end(), userId);
        if (it != likes.end()) { likes.erase(it); return true; }
        return false;
    }

    int getLikeCount() const { return static_cast<int>(likes.size()); }

    // ── Operator overloads ────────────────────────────────────────────────────

    // Equality — compare by message ID
    bool operator==(const Message& other) const {
        return messageId == other.messageId;
    }

    bool operator!=(const Message& other) const {
        return !(*this == other);
    }

    // Less-than — order by timestamp (enables sorting)
    bool operator<(const Message& other) const {
        return timestamp < other.timestamp;
    }

    // Stream output — replaces display()
    friend ostream& operator<<(ostream& os, const Message& m) {
        os << "From: "    << m.senderId << "\n"
           << "Message: " << m.content  << "\n"
           << "Likes: "   << m.likes.size() << "\n"
           << "Time: "    << ctime(&m.timestamp);
        return os;
    }

    // ── Display (delegates to operator<<) ────────────────────────────────────
    void display() const { cout << *this; }

    // ── Serialization ─────────────────────────────────────────────────────────
    string toCSV() const {
        stringstream ss;
        ss << messageId << "," << senderId << "," << content << ","
           << timestamp << "," << static_cast<int>(status) << ",";
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

        getline(ss, msgId,   ',');
        getline(ss, sender,  ',');
        getline(ss, cont,    ',');
        ss >> ts;       ss.ignore();
        ss >> statusInt; ss.ignore();
        getline(ss, likesStr);

        Message msg(msgId, sender, cont);
        msg.timestamp = ts;
        msg.status    = static_cast<MessageStatus>(statusInt);

        if (!likesStr.empty()) {
            stringstream likeSS(likesStr);
            string userId;
            while (getline(likeSS, userId, ';'))
                if (!userId.empty()) msg.likes.push_back(userId);
        }
        return msg;
    }
};

// ============================================================================
// ABSTRACT BASE CLASS — Chat
// Defines the interface shared by Conversation and GroupChat.
// Pure virtual functions make this class abstract (cannot be instantiated).
// ============================================================================
class Chat {
public:
    virtual ~Chat() = default;

    // ── Pure virtual interface (must be overridden) ───────────────────────────
    virtual string                        getChatId()        const = 0;
    virtual bool                          isParticipant(const string& userId) const = 0;
    virtual bool                          addMessage(shared_ptr<Message> msg)  = 0;
    virtual vector<shared_ptr<Message>>   getMessages()      const = 0;
    virtual shared_ptr<Message>           findMessage(const string& msgId)     = 0;
    virtual int                           getMessageCount()  const = 0;
    virtual void                          display()          const = 0;

    // ── Concrete helper available to all subclasses ───────────────────────────
    // Returns the N most recent messages (or all if limit < 0).
    vector<shared_ptr<Message>> getRecentMessages(int limit = -1) const {
        auto msgs = getMessages();
        if (limit < 0 || limit >= static_cast<int>(msgs.size())) return msgs;
        return vector<shared_ptr<Message>>(msgs.end() - limit, msgs.end());
    }

    // ── Operator overload ─────────────────────────────────────────────────────
    // Two chats are equal if they share the same ID.
    bool operator==(const Chat& other) const {
        return getChatId() == other.getChatId();
    }
    bool operator!=(const Chat& other) const { return !(*this == other); }
};

// ============================================================================
// CONVERSATION CLASS  (inherits from Chat — one-on-one)
// ============================================================================
class Conversation : public Chat {
private:
    string                      conversationId;
    vector<string>              participantIds;   // exactly 2
    vector<shared_ptr<Message>> messages;
    time_t                      createdAt;

public:
    Conversation(const string& convId, const string& user1, const string& user2)
        : conversationId(convId), createdAt(time(nullptr)) {
        participantIds.push_back(user1);
        participantIds.push_back(user2);
        sort(participantIds.begin(), participantIds.end());
    }

    // ── Chat interface (polymorphic overrides) ────────────────────────────────
    string getChatId() const override { return conversationId; }

    bool isParticipant(const string& userId) const override {
        return find(participantIds.begin(), participantIds.end(), userId)
               != participantIds.end();
    }

    bool addMessage(shared_ptr<Message> message) override {
        if (!isParticipant(message->getSenderId())) return false;
        messages.push_back(message);
        return true;
    }

    vector<shared_ptr<Message>> getMessages() const override { return messages; }

    shared_ptr<Message> findMessage(const string& messageId) override {
        // Uses the template utility
        return findByKey<Message>(messages,
            [](const shared_ptr<Message>& m){ return m->getMessageId(); },
            messageId);
    }

    int  getMessageCount() const override { return static_cast<int>(messages.size()); }

    void display() const override {
        cout << "\n=== Conversation: " << conversationId << " ===" << endl;
        cout << "Participants: " << participantIds[0] << " <-> " << participantIds[1] << endl;
        cout << "Messages: " << messages.size() << endl;
        cout << "Created: " << ctime(&createdAt);
        for (const auto& msg : messages) { cout << "\n---" << endl; msg->display(); }
    }

    // ── Conversation-specific getters ─────────────────────────────────────────
    string         getConversationId()  const { return conversationId; }
    vector<string> getParticipantIds()  const { return participantIds; }
    time_t         getCreatedAt()       const { return createdAt;      }
};

// ============================================================================
// GROUP CHAT CLASS  (inherits from Chat)
// ============================================================================
class GroupChat : public Chat {
private:
    string                      groupId;
    string                      groupName;
    string                      adminId;
    vector<string>              participantIds;
    vector<shared_ptr<Message>> messages;
    time_t                      createdAt;

public:
    GroupChat(const string& grpId, const string& name, const string& admin)
        : groupId(grpId), groupName(name), adminId(admin), createdAt(time(nullptr)) {
        participantIds.push_back(admin);
    }

    // ── Chat interface (polymorphic overrides) ────────────────────────────────
    string getChatId() const override { return groupId; }

    bool isParticipant(const string& userId) const override {
        return find(participantIds.begin(), participantIds.end(), userId)
               != participantIds.end();
    }

    bool addMessage(shared_ptr<Message> message) override {
        if (!isParticipant(message->getSenderId())) return false;
        messages.push_back(message);
        return true;
    }

    vector<shared_ptr<Message>> getMessages() const override { return messages; }

    shared_ptr<Message> findMessage(const string& messageId) override {
        return findByKey<Message>(messages,
            [](const shared_ptr<Message>& m){ return m->getMessageId(); },
            messageId);
    }

    int  getMessageCount() const override { return static_cast<int>(messages.size()); }

    void display() const override {
        cout << "\n=== Group: " << groupName << " ===" << endl;
        cout << "Group ID: " << groupId << endl;
        cout << "Admin: "    << adminId << endl;
        cout << "Participants (" << participantIds.size() << "): ";
        for (size_t i = 0; i < participantIds.size(); i++) {
            cout << participantIds[i];
            if (i < participantIds.size() - 1) cout << ", ";
        }
        cout << endl;
        cout << "Messages: " << messages.size() << endl;
        cout << "Created: "  << ctime(&createdAt);
        for (const auto& msg : messages) { cout << "\n---" << endl; msg->display(); }
    }

    // ── GroupChat-specific methods ────────────────────────────────────────────
    string         getGroupId()         const { return groupId;           }
    string         getGroupName()       const { return groupName;         }
    string         getAdminId()         const { return adminId;           }
    vector<string> getParticipantIds()  const { return participantIds;    }
    time_t         getCreatedAt()       const { return createdAt;         }
    int            getParticipantCount() const { return static_cast<int>(participantIds.size()); }

    void setGroupName(const string& name) { groupName = name; }

    bool isAdmin(const string& userId) const { return userId == adminId; }

    // Only admin can add; silently ignores if already a member
    bool addParticipant(const string& userId, const string& addedBy) {
        if (!isAdmin(addedBy))          return false;
        if (isParticipant(userId))      return false;
        participantIds.push_back(userId);
        return true;
    }

    // Only admin can remove; cannot remove the admin themselves
    bool removeParticipant(const string& userId, const string& removedBy) {
        if (!isAdmin(removedBy) || userId == adminId) return false;
        auto it = find(participantIds.begin(), participantIds.end(), userId);
        if (it == participantIds.end()) return false;
        participantIds.erase(it);
        return true;
    }
};

#endif // MESSENGER_SYSTEM_H
