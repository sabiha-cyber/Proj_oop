#ifndef FRIEND_SERVICE_H
#define FRIEND_SERVICE_H

#include "AuthenticationService.h"
#include "User.h"
#include <vector>
#include <map>
#include <string>

class FriendService {
private:
    AuthenticationService& authService;

    // Pending requests: senderId → list of receiverIds who have pending request from him
    std::map<int, std::vector<int>> pendingRequests;

    static const std::string REQUESTS_FILE;
    static const std::string FRIENDS_FILE;

    void loadFriendRequests();
    void saveFriendRequests() const;
    void loadFriends();           // ← important: missing in your code
    void saveFriends() const;

public:
    explicit FriendService(AuthenticationService& auth);

    bool sendFriendRequest(int senderId, const std::string& targetUsername);

    bool acceptFriendRequest(int receiverId, int senderId);

    bool rejectFriendRequest(int receiverId, int senderId);

    void showPendingRequestsForUser(int userId) const;

    bool areFriends(int userId1, int userId2) const;

    std::vector<int> getFriendIdsOf(int userId) const;   // changed to return IDs (safer)
};

#endif // FRIEND_SERVICE_H