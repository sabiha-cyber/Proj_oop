#ifndef FRIEND_SERVICE_H
#define FRIEND_SERVICE_H

#include "AuthenticationService.h"
#include "User.h"
#include <vector>
#include <map>
#include <string>

class NotificationManager;
class FriendService {
private:
    AuthenticationService& authService;
    NotificationManager*   notifMgr=nullptr;


    std::map<int, std::vector<int>> pendingRequests;

    static const std::string REQUESTS_FILE;
    static const std::string FRIENDS_FILE;

    void loadFriendRequests();
    void saveFriendRequests() const;
    void loadFriends();           
    void saveFriends() const;

public:
    explicit FriendService(AuthenticationService& auth, NotificationManager* notifMgr = nullptr);

    bool sendFriendRequest(int senderId, const std::string& targetUsername);

    bool acceptFriendRequest(int receiverId, int senderId);

    bool rejectFriendRequest(int receiverId, int senderId);
     bool removeFriend(int userId, int friendId);

    void showPendingRequestsForUser(int userId) const;

    bool areFriends(int userId1, int userId2) const;
    void debugDumpPending() const;
    std::vector<int> getFriendIdsOf(int userId) const;   
    FriendService(const FriendService&) = delete;
    FriendService& operator=(const FriendService&) = delete;
};

#endif 
