#include "FriendService.h"
#include <fstream>
#include <iostream>
#include <algorithm>

const std::string FriendService::REQUESTS_FILE = "friend_requests.txt";
const std::string FriendService::FRIENDS_FILE  = "friends.txt";

FriendService::FriendService(AuthenticationService& auth) : authService(auth) {
    loadFriendRequests();
    loadFriends();  // ← crucial: load existing friendships
}

void FriendService::loadFriendRequests() {
    std::ifstream file(REQUESTS_FILE);
    if (!file.is_open()) return;

    int sender, receiver;
    while (file >> sender >> receiver) {
        pendingRequests[sender].push_back(receiver);
    }
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
            user2->addFriend(u1);  // ensure both directions
        }
    }
    file.close();
}

void FriendService::saveFriendRequests() const {
    std::ofstream file(REQUESTS_FILE);
    if (!file.is_open()) return;

    for (const auto& [sender, recvs] : pendingRequests) {
        for (int r : recvs) {
            file << sender << " " << r << "\n";
        }
    }
    file.close();
}

void FriendService::saveFriends() const {
    std::ofstream file(FRIENDS_FILE);
    if (!file.is_open()) return;

    const auto& users = authService.getUsers();
    //std::set<std::pair<int, int>> savedPairs;  // avoid duplicates

    for (const auto& u : users) {
        int uid = u.getUserId();
        for (int fid : u.getFriendIds()) {
            if (uid < fid) {
                file << uid << " " << fid << "\n";
            } else if (fid < uid) {
                file << fid << " " << uid << "\n";
            }
        }
    }
    file.close();
}

bool FriendService::sendFriendRequest(int senderId, const std::string& targetUsername) {
    User* sender = authService.findUserById(senderId);
    User* target = authService.findUserByUsername(targetUsername);

    if (!sender || !target) {
        std::cout << "User not found.\n";
        return false;
    }

    int targetId = target->getUserId();

    if (senderId == targetId) {
        std::cout << "Cannot send request to yourself.\n";
        return false;
    }

    if (areFriends(senderId, targetId)) {
        std::cout << "Already friends.\n";
        return false;
    }

    auto& sent = pendingRequests[senderId];
    if (std::find(sent.begin(), sent.end(), targetId) != sent.end()) {
        std::cout << "Request already sent.\n";
        return false;
    }

    // Check if target already sent request to sender → auto-accept
    auto& incoming = pendingRequests[targetId];
    auto it = std::find(incoming.begin(), incoming.end(), senderId);
    if (it != incoming.end()) {
        incoming.erase(it);
        sender->addFriend(targetId);
        target->addFriend(senderId);
        saveFriends();
        saveFriendRequests();
        std::cout << "Mutual request! You are now friends.\n";
        return true;
    }

    sent.push_back(targetId);
    saveFriendRequests();
    std::cout << "Friend request sent to " << targetUsername << ".\n";
    return true;
}

bool FriendService::acceptFriendRequest(int receiverId, int senderId) {
    auto& sentBySender = pendingRequests[senderId];
    auto it = std::find(sentBySender.begin(), sentBySender.end(), receiverId);
    if (it == sentBySender.end()) {
        std::cout << "No pending request from that user.\n";
        return false;
    }

    sentBySender.erase(it);

    User* receiver = authService.findUserById(receiverId);
    User* sender   = authService.findUserById(senderId);

    if (!receiver || !sender) return false;

    receiver->addFriend(senderId);
    sender->addFriend(receiverId);

    saveFriends();
    saveFriendRequests();

    std::cout << "Friend request accepted. You are now friends.\n";
    return true;
}

bool FriendService::rejectFriendRequest(int receiverId, int senderId) {
    auto& sentBySender = pendingRequests[senderId];
    auto it = std::find(sentBySender.begin(), sentBySender.end(), receiverId);
    if (it == sentBySender.end()) {
        std::cout << "No such pending request.\n";
        return false;
    }

    sentBySender.erase(it);
    saveFriendRequests();
    std::cout << "Friend request rejected.\n";
    return true;
}

void FriendService::showPendingRequestsForUser(int userId) const {
    std::cout << "\nPending friend requests for you:\n";
    bool hasAny = false;

    for (const auto& [senderId, receivers] : pendingRequests) {
        if (std::find(receivers.begin(), receivers.end(), userId) != receivers.end()) {
            User* sender = authService.findUserById(senderId);
            if (sender) {
                std::cout << "  - From: " << sender->getUsername()
                          << " (ID: " << senderId << ")\n";
                hasAny = true;
            }
        }
    }

    if (!hasAny) {
        std::cout << "  No pending requests.\n";
    }
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