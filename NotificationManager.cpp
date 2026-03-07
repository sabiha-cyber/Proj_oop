#include "NotificationManager.h"
#include "Notification.h"

// Real manager headers — NotificationManager reads from these
#include "LikeManager.h"
#include "CommentManager.h"
#include "PostManager.h"
#include "AuthenticationService.h"
#include "FriendService.h"

// Real entity headers
#include "Like.h"
#include "Comment.h"
#include "Post.h"
#include "User.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

const string NotificationManager::NOTIF_FILE = "notifications.txt";

// ── Constructor ───────────────────────────────────────────────────────────────
NotificationManager::NotificationManager(LikeManager*           likeMgr,
                                          CommentManager*        commentMgr,
                                          PostManager*           postMgr,
                                          AuthenticationService* authSvc,
                                          FriendService*         friendSvc)
    : nextId(1),
      likeMgr(likeMgr),
      commentMgr(commentMgr),
      postMgr(postMgr),
      authSvc(authSvc),
      friendSvc(friendSvc)
{
    loadFromFile();
}

NotificationManager::~NotificationManager() {
    clear();
}

// ── alreadyExists: prevents duplicate notifications on re-scan ────────────────
bool NotificationManager::alreadyExists(NotificationType type,
                                         int actorId,
                                         int referenceId) const {
    for (const Notification* n : notifications) {
        if (n->getType()        == type      &&
            n->getActorId()     == actorId   &&
            n->getReferenceId() == referenceId)
            return true;
    }
    return false;
}

// ── scanLikes ─────────────────────────────────────────────────────────────────
// Reads every Like from LikeManager, finds the post owner, creates notification
void NotificationManager::scanLikes() {
    if (!likeMgr) return;

    for (Like* l : likeMgr->getAllLikes()) {
        if (!l || !l->getUser() || !l->getPost()) continue;

        Post* post   = l->getPost();
        User* actor  = l->getUser();
        User* owner  = post->getAuthor();

        if (!owner || owner == actor) continue;          // skip self-like
        if (post->isDeletedPost())    continue;

        if (!alreadyExists(NotificationType::LIKE,
                            actor->getUserId(),
                            post->getPostId()))
        {
            create(owner->getUserId(),
                   actor->getUserId(),
                   NotificationType::LIKE,
                   post->getPostId(),
                   actor->getUsername());
        }
    }
}

// ── scanComments ──────────────────────────────────────────────────────────────
void NotificationManager::scanComments() {
    if (!commentMgr) return;

    for (Comment* c : commentMgr->getAllComments()) {
        if (!c || !c->getAuthor() || !c->getPost()) continue;
        if (c->isDeletedComment()) continue;

        Post* post   = c->getPost();
        User* actor  = c->getAuthor();
        User* owner  = post->getAuthor();

        if (!owner || owner == actor) continue;
        if (post->isDeletedPost())    continue;

        if (!alreadyExists(NotificationType::COMMENT,
                            actor->getUserId(),
                            post->getPostId()))
        {
            string snippet = c->getText().substr(0, 60);
            create(owner->getUserId(),
                   actor->getUserId(),
                   NotificationType::COMMENT,
                   post->getPostId(),
                   actor->getUsername(),
                   snippet);
        }
    }
}

// ── scanFriends ───────────────────────────────────────────────────────────────
// Reads confirmed friendships from FriendService and generates FRIEND_ACCEPT
// notifications for both sides if not already present.
void NotificationManager::scanFriends() {
    if (!friendSvc || !authSvc) return;

    for (const User& u : authSvc->getUsers()) {
        int uid = u.getUserId();
        for (int fid : u.getFriendIds()) {
            if (uid >= fid) continue;   // process each pair once

            User* a = authSvc->findUserById(uid);
            User* b = authSvc->findUserById(fid);
            if (!a || !b) continue;

            // Notify uid that fid accepted (if not already there)
            if (!alreadyExists(NotificationType::FRIEND_ACCEPT, fid, fid))
                create(uid, fid, NotificationType::FRIEND_ACCEPT,
                       fid, b->getUsername());

            // Notify fid that uid accepted (if not already there)
            if (!alreadyExists(NotificationType::FRIEND_ACCEPT, uid, uid))
                create(fid, uid, NotificationType::FRIEND_ACCEPT,
                       uid, a->getUsername());
        }
    }
}

// ── scanAndGenerate ───────────────────────────────────────────────────────────
// Call this once after all managers are loaded from their files.
// Reads real data and fills in any missing notifications.
void NotificationManager::scanAndGenerate() {
    scanLikes();
    scanComments();
    scanFriends();
    saveToFile();   // persist newly generated notifications
    cout << "[NotificationManager] Scan complete. Total notifications: "
         << notifications.size() << "\n";
}

// ── Internal factory ──────────────────────────────────────────────────────────
Notification* NotificationManager::create(int recipientId, int actorId,
                                           NotificationType type, int referenceId,
                                           const string& actorUsername,
                                           const string& snippet) {
    if (recipientId == actorId) return nullptr;   // never self-notify

    Notification* n = new Notification(nextId++, recipientId, actorId,
                                        type, referenceId,
                                        actorUsername, snippet);
    notifications.push_back(n);
    return n;
}

// ── Direct notify methods (called right after a new action at runtime) ────────
Notification* NotificationManager::notifyLike(int recipientId, int actorId,
                                               const string& actorUsername,
                                               int postId) {
    if (alreadyExists(NotificationType::LIKE, actorId, postId)) return nullptr;
    Notification* n = create(recipientId, actorId,
                              NotificationType::LIKE, postId, actorUsername);
    saveToFile();
    return n;
}

Notification* NotificationManager::notifyComment(int recipientId, int actorId,
                                                   const string& actorUsername,
                                                   int postId,
                                                   const string& snippet) {
    string s = snippet.substr(0, 60);
    Notification* n = create(recipientId, actorId,
                              NotificationType::COMMENT, postId, actorUsername, s);
    saveToFile();
    return n;
}

Notification* NotificationManager::notifyFriendRequest(int recipientId, int actorId,
                                                         const string& actorUsername) {
    if (alreadyExists(NotificationType::FRIEND_REQUEST, actorId, actorId))
        return nullptr;
    Notification* n = create(recipientId, actorId,
                              NotificationType::FRIEND_REQUEST, actorId, actorUsername);
    saveToFile();
    return n;
}

Notification* NotificationManager::notifyFriendAccept(int recipientId, int actorId,
                                                        const string& actorUsername) {
    Notification* n = create(recipientId, actorId,
                              NotificationType::FRIEND_ACCEPT, actorId, actorUsername);
    saveToFile();
    return n;
}

Notification* NotificationManager::notifyShare(int recipientId, int actorId,
                                                const string& actorUsername,
                                                int postId) {
    Notification* n = create(recipientId, actorId,
                              NotificationType::POST_SHARE, postId, actorUsername);
    saveToFile();
    return n;
}

Notification* NotificationManager::notifyMention(int recipientId, int actorId,
                                                   const string& actorUsername,
                                                   int postId,
                                                   const string& snippet) {
    string s = snippet.substr(0, 60);
    Notification* n = create(recipientId, actorId,
                              NotificationType::MENTION, postId, actorUsername, s);
    saveToFile();
    return n;
}

// ── Inbox display ─────────────────────────────────────────────────────────────
void NotificationManager::showInbox(int userId) const {
    auto notifs = getForUser(userId);

    cout << "\n +------------------------------------------+\n";
    cout << "|           NOTIFICATIONS                      |\n";
    cout << " +------------------------------------------+\n";

    int unread = unreadCount(userId);
    if (unread > 0) cout << "  " << unread << " unread\n\n";

    if (notifs.empty()) { cout << "  No notifications yet.\n"; return; }

    for (Notification* n : notifs) n->display();
    cout << "\n";
}

void NotificationManager::showUnread(int userId) const {
    auto notifs = getUnreadForUser(userId);
    cout << "\n── Unread Notifications ──────────────────────\n";
    if (notifs.empty()) { cout << "  All caught up!\n"; return; }
    for (Notification* n : notifs) n->display();
    cout << "\n";
}

int NotificationManager::unreadCount(int userId) const {
    int count = 0;
    for (const Notification* n : notifications)
        if (n->getRecipientId() == userId && !n->isRead()) ++count;
    return count;
}

// ── Read management ───────────────────────────────────────────────────────────
bool NotificationManager::markAsRead(int notifId) {
    Notification* n = findById(notifId);
    if (!n) return false;
    n->markAsRead();
    saveToFile();
    return true;
}

void NotificationManager::markAllAsRead(int userId) {
    for (Notification* n : notifications)
        if (n->getRecipientId() == userId && !n->isRead()) n->markAsRead();
    saveToFile();
    cout << "All notifications marked as read.\n";
}

// ── Deletion ──────────────────────────────────────────────────────────────────
bool NotificationManager::deleteById(int notifId) {
    for (auto it = notifications.begin(); it != notifications.end(); ++it) {
        if ((*it)->getNotifId() == notifId) {
            delete *it;
            notifications.erase(it);
            saveToFile();
            cout << "Notification #" << notifId << " deleted.\n";
            return true;
        }
    }
    cout << "Notification #" << notifId << " not found.\n";
    return false;
}

void NotificationManager::deleteAllForUser(int userId) {
    int count = 0;
    for (auto it = notifications.begin(); it != notifications.end(); ) {
        if ((*it)->getRecipientId() == userId) {
            delete *it; it = notifications.erase(it); ++count;
        } else { ++it; }
    }
    saveToFile();
    cout << count << " notification(s) cleared.\n";
}

void NotificationManager::deleteByType(int userId, NotificationType type) {
    int count = 0;
    for (auto it = notifications.begin(); it != notifications.end(); ) {
        if ((*it)->getRecipientId() == userId && (*it)->getType() == type) {
            delete *it; it = notifications.erase(it); ++count;
        } else { ++it; }
    }
    saveToFile();
    cout << count << " notification(s) of type '"
         << Notification::typeToString(type) << "' cleared.\n";
}

// ── Lookup ────────────────────────────────────────────────────────────────────
Notification* NotificationManager::findById(int notifId) const {
    for (Notification* n : notifications)
        if (n->getNotifId() == notifId) return n;
    return nullptr;
}

vector<Notification*> NotificationManager::getForUser(int userId) const {
    vector<Notification*> result;
    for (Notification* n : notifications)
        if (n->getRecipientId() == userId) result.push_back(n);
    sort(result.begin(), result.end(), [](const Notification* a, const Notification* b) {
        return a->getCreatedAt() > b->getCreatedAt();
    });
    return result;
}

vector<Notification*> NotificationManager::getUnreadForUser(int userId) const {
    vector<Notification*> result;
    for (Notification* n : notifications)
        if (n->getRecipientId() == userId && !n->isRead()) result.push_back(n);
    sort(result.begin(), result.end(), [](const Notification* a, const Notification* b) {
        return a->getCreatedAt() > b->getCreatedAt();
    });
    return result;
}

// ── Persistence ───────────────────────────────────────────────────────────────
void NotificationManager::saveToFile(const string& filename) const {
    ofstream out(filename);
    if (!out) { cerr << "Warning: Could not open '" << filename << "'.\n"; return; }

    for (const Notification* n : notifications) {
        out << n->getNotifId()                           << '|'
            << n->getRecipientId()                       << '|'
            << n->getActorId()                           << '|'
            << Notification::typeToString(n->getType())  << '|'
            << n->getReferenceId()                       << '|'
            << n->getActorUsername()                     << '|'
            << (n->isRead() ? 1 : 0)                    << '|'
            << static_cast<long long>(n->getCreatedAt()) << '|'
            << n->getSnippet()                           << '\n';
    }
}

void NotificationManager::loadFromFile(const string& filename) {
    ifstream in(filename);
    if (!in) return;

    clear();
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);

        auto nextField = [&](string& out) -> bool {
            return static_cast<bool>(getline(ss, out, '|'));
        };

        string notifIdStr, recipStr, actorStr, typeStr,
               refStr, actorUser, readStr, timeStr, snippet;

        if (!nextField(notifIdStr) || !nextField(recipStr)  ||
            !nextField(actorStr)   || !nextField(typeStr)   ||
            !nextField(refStr)     || !nextField(actorUser) ||
            !nextField(readStr)    || !nextField(timeStr))   continue;

        getline(ss, snippet);

        int    notifId   = stoi(notifIdStr);
        int    recipId   = stoi(recipStr);
        int    actorId   = stoi(actorStr);
        int    refId     = stoi(refStr);
        bool   read      = (stoi(readStr) != 0);
        time_t createdAt = static_cast<time_t>(stoll(timeStr));
        NotificationType type = Notification::stringToType(typeStr);

        Notification* n = new Notification(notifId, recipId, actorId,
                                            type, refId,
                                            actorUser, snippet,
                                            createdAt, read);
        notifications.push_back(n);
        if (notifId >= nextId) nextId = notifId + 1;
    }
}

// ── Misc ──────────────────────────────────────────────────────────────────────
void NotificationManager::clear() {
    for (Notification* n : notifications) delete n;
    notifications.clear();
    nextId = 1;
}

const vector<Notification*>& NotificationManager::getAll() const {
    return notifications;
}