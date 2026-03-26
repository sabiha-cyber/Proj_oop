#include "FriendService.h"
#include "NotificationManager.h"

#include <fstream>
#include <iostream>
#include <algorithm>

const std::string FriendService::REQUESTS_FILE = "friend_requests.txt";
const std::string FriendService::FRIENDS_FILE  = "friends.txt";

FriendService::FriendService(AuthenticationService& auth,
                              NotificationManager* notifMgr)
    : authService(auth), notifMgr(notifMgr)
{
    loadFriendRequests();
    loadFriends();
}

void FriendService::loadFriendRequests() {
    std::ifstream file(REQUESTS_FILE);
    if (!file.is_open()) return;
    int sender, receiver;
    while (file >> sender >> receiver)
        pendingRequests[sender].push_back(receiver);
    file.close();
}

void FriendService::loadFriends() {
    std::ifstream file(FRIENDS_FILE);
    if (!file.is_open()) return;
    int u1, u2;
    while (file >> u1 >> u2) {
        User* user1 = authService.findUserById(u1);
        User* user2 = authService.findUserById(u2);
        if (user1 && user2) {
            user1->addFriend(u2);
            user2->addFriend(u1);
        }
    }
    file.close();
}

void FriendService::saveFriendRequests() const {
    std::ofstream file(REQUESTS_FILE);
    if (!file.is_open()) return;
    for (const auto& [sender, recvs] : pendingRequests)
        for (int r : recvs)
            file << sender << " " << r << "\n";
    file.close();
}

void FriendService::saveFriends() const {
    std::ofstream file(FRIENDS_FILE);
    if (!file.is_open()) return;
    const auto& users = authService.getUsers();
    for (const auto& u : users) {
        int uid = u.getUserId();
        for (int fid : u.getFriendIds())
            if (uid < fid)
                file << uid << " " << fid << "\n";
    }
    file.close();
}

bool FriendService::sendFriendRequest(int senderId,
                                       const std::string& targetUsername) {
    User* sender = authService.findUserById(senderId);
    User* target = authService.findUserByUsername(targetUsername);

    if (!sender || !target) { std::cout << "User not found.\n"; return false; }

    int targetId = target->getUserId();

    if (senderId == targetId)         { std::cout << "Cannot send request to yourself.\n"; return false; }
    if (areFriends(senderId,targetId)){ std::cout << "Already friends.\n"; return false; }

    auto& sent = pendingRequests[senderId];
    if (std::find(sent.begin(), sent.end(), targetId) != sent.end()) {
        std::cout << "Request already sent.\n"; return false;
    }

    auto& incoming = pendingRequests[targetId];
    auto it = std::find(incoming.begin(), incoming.end(), senderId);
    if (it != incoming.end()) {
        incoming.erase(it);
        sender->addFriend(targetId);
        target->addFriend(senderId);
        saveFriends();
        saveFriendRequests();
      
        std::cout << "DEBUG sendFriendRequest this=" << this
          << " senderId=" << senderId
          << " targetId=" << targetId << "\n";
        debugDumpPending();

        if (notifMgr) {
            notifMgr->notifyFriendAccept(senderId,  targetId, target->getUsername());
            notifMgr->notifyFriendAccept(targetId,  senderId, sender->getUsername());
        }
        std::cout << "Mutual request! You are now friends.\n";
        return true;
    }

    sent.push_back(targetId);
    saveFriendRequests();

    if (notifMgr) {
        notifMgr->notifyFriendRequest(targetId, senderId, sender->getUsername());
    }

    std::cout << "Friend request sent to " << targetUsername << ".\n";
    return true;
}

bool FriendService::acceptFriendRequest(int receiverId, int senderId)
{

    auto itMap = pendingRequests.find(senderId);
    if (itMap == pendingRequests.end()) {
        std::cout << "No pending request from that user.\n";
        return false;
    }

    // Verify that receiverId is actually in the sender's pending list
    auto& receivers = itMap->second;
    auto it = std::find(receivers.begin(), receivers.end(), receiverId);
    if (it == receivers.end()) {
        std::cout << "No pending request from that user.\n";
        return false;
    }

    User* receiver = authService.findUserById(receiverId);
    User* sender   = authService.findUserById(senderId);
    if (!receiver || !sender) {
        std::cout << "User not found. Cannot accept request.\n";
        return false;
    }

    if (areFriends(receiverId, senderId)) {
        receivers.erase(it);
        if (receivers.empty()) pendingRequests.erase(itMap);
        saveFriendRequests();
        std::cout << "You are already friends. Pending request removed.\n";
        return true;
    }

    receivers.erase(it);
    if (receivers.empty()) pendingRequests.erase(itMap);

    receiver->addFriend(senderId);
    sender->addFriend(receiverId);

    saveFriends();
    saveFriendRequests();

    if (notifMgr) {
        notifMgr->notifyFriendAccept(senderId, receiverId, receiver->getUsername());
    }

    std::cout << "Friend request accepted. You are now friends.\n";
    return true;
}

bool FriendService::rejectFriendRequest(int receiverId, int senderId) {
    auto& sentBySender = pendingRequests[senderId];
    auto it = std::find(sentBySender.begin(), sentBySender.end(), receiverId);
    if (it == sentBySender.end()) {
        std::cout << "No such pending request.\n"; return false;
    }
    sentBySender.erase(it);
    saveFriendRequests();
    std::cout << "Friend request rejected.\n";
    return true;
}

bool FriendService::removeFriend(int userId, int friendId) {
    User* user = authService.findUserById(userId);
    User* friendUser = authService.findUserById(friendId);

    if (!user || !friendUser) {
        std::cout << "  One of the users not found.\n";
        return false;
    }

    if (!areFriends(userId, friendId)) {
        std::cout << "  You are not friends with this user.\n";
        return false;
    }

    user->removeFriend(friendId);
    friendUser->removeFriend(userId);

    saveFriends();

    std::cout << "  You have removed " << friendUser->getUsername() << " from your friends list.\n";


    return true;
}

void FriendService::showPendingRequestsForUser(int userId) const {
  
    std::cout << "DEBUG showPendingRequestsForUser this=" << this
          << " userId=" << userId << "\n";
        debugDumpPending();
    
    std::cout << "\nPending friend requests for you:\n";
    bool hasAny = false;

    for (const auto& [senderId, receivers] : pendingRequests) {
        if (std::find(receivers.begin(), receivers.end(), userId) != receivers.end()) {
            hasAny = true;

            if (User* sender = authService.findUserById(senderId)) {
                std::cout << "  - From: " << sender->getUsername()
                          << " (ID: " << senderId << ")\n";
            } else {
                std::cout << "  - From user ID: " << senderId
                          << " (sender not found in authService)\n";
            }
        }
    }

    if (!hasAny) std::cout << "  No pending requests.\n";
}

bool FriendService::areFriends(int userId1, int userId2) const {
    User* u = authService.findUserById(userId1);
    if (!u) return false;
    return u->hasFriend(userId2);
}

std::vector<int> FriendService::getFriendIdsOf(int userId) const {
    User* u = authService.findUserById(userId);
    if (!u) return {};
    return u->getFriendIds();
}

void FriendService::debugDumpPending() const
{
    std::cout << "\n--- DEBUG pendingRequests (this=" << this << ") ---\n";
    std::cout << "size = " << pendingRequests.size() << "\n";

    for (const auto& entry : pendingRequests) {
        int senderId = entry.first;
        const auto& receivers = entry.second;

        std::cout << "sender " << senderId << " -> [ ";
        for (int r : receivers) std::cout << r << " ";
        std::cout << "]\n";
    }
    std::cout << "----------------------------------------------------------\n";
}
