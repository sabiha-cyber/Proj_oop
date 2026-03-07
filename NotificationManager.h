#pragma once

#include "Notification.h"

#include <vector>
#include <string>

// Forward declarations — no circular includes
class LikeManager;
class CommentManager;
class PostManager;
class AuthenticationService;
class FriendService;
class User;
class Post;
class Like;
class Comment;

// ── NotificationManager ───────────────────────────────────────────────────────
// Owns all Notification* objects.
// Holds references to the real managers so it can READ from them and
// auto-generate notifications based on what happened in those classes.
// The managers themselves are NOT modified at all.
// ─────────────────────────────────────────────────────────────────────────────
class NotificationManager {
public:
    static const std::string NOTIF_FILE;

    // ── Constructor: wire up to real managers ─────────────────────────────────
    // All pointers are optional (pass nullptr if not available)
    NotificationManager(LikeManager*            likeMgr   = nullptr,
                        CommentManager*         commentMgr = nullptr,
                        PostManager*            postMgr    = nullptr,
                        AuthenticationService*  authSvc    = nullptr,
                        FriendService*          friendSvc  = nullptr);
    ~NotificationManager();

    // ── Scan real managers and generate notifications from current state ───────
    // Call once after loading all managers from file at startup.
    // Compares what's in LikeManager / CommentManager against already-saved
    // notifications and fills in any that are missing.
    void scanAndGenerate();

    // ── Direct notify methods (called when a NEW action happens at runtime) ────
    // These are still available so you can call them right after an action.
    Notification* notifyLike          (int recipientId, int actorId,
                                        const std::string& actorUsername, int postId);

    Notification* notifyComment       (int recipientId, int actorId,
                                        const std::string& actorUsername,
                                        int postId, const std::string& snippet = "");

    Notification* notifyFriendRequest (int recipientId, int actorId,
                                        const std::string& actorUsername);

    Notification* notifyFriendAccept  (int recipientId, int actorId,
                                        const std::string& actorUsername);

    Notification* notifyShare         (int recipientId, int actorId,
                                        const std::string& actorUsername, int postId);

    Notification* notifyMention       (int recipientId, int actorId,
                                        const std::string& actorUsername,
                                        int postId, const std::string& snippet = "");

    // ── Inbox display ─────────────────────────────────────────────────────────
    void showInbox  (int userId) const;
    void showUnread (int userId) const;
    int  unreadCount(int userId) const;

    // ── Read management ───────────────────────────────────────────────────────
    bool markAsRead    (int notifId);
    void markAllAsRead (int userId);

    // ── Deletion ──────────────────────────────────────────────────────────────
    bool deleteById        (int notifId);
    void deleteAllForUser  (int userId);
    void deleteByType      (int userId, NotificationType type);

    // ── Lookup ────────────────────────────────────────────────────────────────
    Notification*              findById         (int notifId) const;
    std::vector<Notification*> getForUser       (int userId)  const;
    std::vector<Notification*> getUnreadForUser (int userId)  const;

    // ── Persistence ───────────────────────────────────────────────────────────
    void saveToFile  (const std::string& filename = NOTIF_FILE) const;
    void loadFromFile(const std::string& filename = NOTIF_FILE);

    // ── Misc ──────────────────────────────────────────────────────────────────
    void clear();
    const std::vector<Notification*>& getAll() const;

private:
    std::vector<Notification*> notifications;
    int nextId;

    // References to real managers (not owned, never deleted here)
    LikeManager*           likeMgr;
    CommentManager*        commentMgr;
    PostManager*           postMgr;
    AuthenticationService* authSvc;
    FriendService*         friendSvc;

    // Internal factory
    Notification* create(int recipientId, int actorId,
                          NotificationType type, int referenceId,
                          const std::string& actorUsername,
                          const std::string& snippet = "");

    // Check if a notification for this exact event already exists
    bool alreadyExists(NotificationType type, int actorId, int referenceId) const;

    // Scan helpers — one per source
    void scanLikes();
    void scanComments();
    void scanFriends();
};