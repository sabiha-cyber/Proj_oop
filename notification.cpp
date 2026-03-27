#include "Notification.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

// ──constructor──
Notification::Notification(int notifId,
                            int recipientUserId,
                            int actorUserId,
                            NotificationType type,
                            int referenceId,
                            const string& actorUsername,
                            const string& snippet,
                            time_t createdAt,
                            bool read)
    : notifId(notifId),
      recipientUserId(recipientUserId),
      actorUserId(actorUserId),
      type(type),
      referenceId(referenceId),
      actorUsername(actorUsername),
      snippet(snippet),
      createdAt(createdAt),
      read(read)
{}

// ──  constructor ──
Notification::Notification(int notifId,
                            int recipientUserId,
                            int actorUserId,
                            NotificationType type,
                            int referenceId,
                            const string& actorUsername,
                            const string& snippet)
    : notifId(notifId),
      recipientUserId(recipientUserId),
      actorUserId(actorUserId),
      type(type),
      referenceId(referenceId),
      actorUsername(actorUsername),
      snippet(snippet),
      createdAt(time(nullptr)),
      read(false)
{}


void Notification::markAsRead() {
    read = true;
}


string Notification::buildMessage() const {
    switch (type) {
        case NotificationType::LIKE:
            return actorUsername + " liked your post.";

        case NotificationType::COMMENT:
            if (snippet.empty())
                return actorUsername + " commented on your post.";
            return actorUsername + " commented: \"" + snippet + "\"";

        case NotificationType::FRIEND_REQUEST:
            return actorUsername + " sent you a friend request.";

        case NotificationType::FRIEND_ACCEPT:
            return actorUsername + " accepted your friend request.";

        case NotificationType::POST_SHARE:
            return actorUsername + " shared your post.";

        case NotificationType::MENTION:
            if (snippet.empty())
                return actorUsername + " mentioned you in a comment.";
            return actorUsername + " mentioned you: \"" + snippet + "\"";

        default:
            return "You have a new notification.";
    }
}

void Notification::display() const {
    tm* ti = localtime(&createdAt);
    char timeBuf[16];
    strftime(timeBuf, sizeof(timeBuf), "%H:%M", ti);

    char dateBuf[12];
    strftime(dateBuf, sizeof(dateBuf), "%b %d", ti);

    cout << (read ? "  [READ] " : "  [NEW] ")
         << left << setw(9) << dateBuf
         << setw(7) << timeBuf
         << buildMessage() << "\n";
}

void Notification::displayDetailed() const {
    tm* ti = localtime(&createdAt);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ti);

    cout << "Notification #" << notifId << "\n"
         << "  Type      : " << typeToString(type)  << "\n"
         << "  Actor     : " << actorUsername        << " (ID " << actorUserId << ")\n"
         << "  Reference : " << referenceId          << "\n"
         << "  Snippet   : " << snippet              << "\n"
         << "  Created   : " << buf                  << "\n"
         << "  Read      : " << (read ? "yes" : "no") << "\n";
}

// ── Static helpers─────────
string Notification::typeToString(NotificationType t) {
    switch (t) {
        case NotificationType::LIKE:           return "LIKE";
        case NotificationType::COMMENT:        return "COMMENT";
        case NotificationType::FRIEND_REQUEST: return "FRIEND_REQUEST";
        case NotificationType::FRIEND_ACCEPT:  return "FRIEND_ACCEPT";
        case NotificationType::POST_SHARE:     return "POST_SHARE";
        case NotificationType::MENTION:        return "MENTION";
        default:                               return "UNKNOWN";
    }
}

NotificationType Notification::stringToType(const string& s) {
    if (s == "LIKE")           return NotificationType::LIKE;
    if (s == "COMMENT")        return NotificationType::COMMENT;
    if (s == "FRIEND_REQUEST") return NotificationType::FRIEND_REQUEST;
    if (s == "FRIEND_ACCEPT")  return NotificationType::FRIEND_ACCEPT;
    if (s == "POST_SHARE")     return NotificationType::POST_SHARE;
    if (s == "MENTION")        return NotificationType::MENTION;
    return NotificationType::LIKE;
}

// ── Getters 
int              Notification::getNotifId()       const { return notifId; }
int              Notification::getRecipientId()   const { return recipientUserId; }
int              Notification::getActorId()       const { return actorUserId; }
NotificationType Notification::getType()          const { return type; }
int              Notification::getReferenceId()   const { return referenceId; }
string           Notification::getActorUsername() const { return actorUsername; }
string           Notification::getSnippet()       const { return snippet; }
time_t           Notification::getCreatedAt()     const { return createdAt; }

bool             Notification::isRead()           const { return read; }
