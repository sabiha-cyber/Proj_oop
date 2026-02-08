#ifndef USER_H
#define USER_H

#include <string>
#include <vector>

class User {
private:
    int userId;
    std::string username;
    std::string password;
    std::vector<int> friendIds;           // still here – most practical for now

public:
    User();
    User(int id, const std::string& uname, const std::string& pass);

    // Getters
    int                 getUserId()     const;
    std::string         getUsername()   const;
    std::string         getPassword()   const;

    // Friend list access (read-only)
    const std::vector<int>& getFriendIds() const;

    // Friend management – called only by FriendService
    void addFriend(int friendUserId);
    bool hasFriend(int otherId) const;

    // Display
    void printBasicInfo() const;
};

#endif // USER_H