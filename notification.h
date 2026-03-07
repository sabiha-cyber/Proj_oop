#pragma once

#include <string>
#include <ctime>

// ── Notification Types ────────────────────────────────────────────────────────
enum class NotificationType {
    LIKE,           // Someone liked your post
    COMMENT,        // Someone commented on your post
    FRIEND_REQUEST, // Someone sent you a friend request
    FRIEND_ACCEPT,  // Someone accepted your friend request
    POST_SHARE,     // Someone shared your post
    MENTION         // Someone mentioned you in a comment
};

// ── Notification Class ────────────────────────────────────────────────────────
class Notification {
public:
    // ── Full constructor (used when loading from file) ────────────────────────
    Notification(int notifId,
                 int recipientUserId,
                 int actorUserId,
                 NotificationType type,
                 int referenceId,          // postId, commentId, or senderId depending on type
                 const std::string& actorUsername,
                 const std::string& snippet,  // e.g. first N chars of comment text
                 time_t createdAt,
                 bool read);

    // ── Factory constructor (used when creating a fresh notification) ─────────
    Notification(int notifId,
                 int recipientUserId,
                 int actorUserId,
                 NotificationType type,
                 int referenceId,
                 const std::string& actorUsername,
                 const std::string& snippet = "");

    // ── Actions ───────────────────────────────────────────────────────────────
    void markAsRead();
    void display() const;                  // one-liner FB-style display
    void displayDetailed() const;          // multi-field debug view

    // ── Static helpers ────────────────────────────────────────────────────────
    static std::string typeToString(NotificationType t);
    static NotificationType stringToType(const std::string& s);

    // ── Getters ───────────────────────────────────────────────────────────────
    int              getNotifId()       const;
    int              getRecipientId()   const;
    int              getActorId()       const;
    NotificationType getType()          const;
    int              getReferenceId()   const;
    std::string      getActorUsername() const;
    std::string      getSnippet()       const;
    time_t           getCreatedAt()     const;
    bool             isRead()           const;

private:
    int              notifId;
    int              recipientUserId;
    int              actorUserId;
    NotificationType type;
    int              referenceId;       // contextual: postId / commentId / friendId
    std::string      actorUsername;
    std::string      snippet;           // optional short preview text
    time_t           createdAt;
    bool             read;

    std::string buildMessage() const;   // builds human-readable string
};