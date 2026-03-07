#include "User.h"
#include <iostream>
#include <algorithm>

User::User() : userId(-1) {}

User::User(int id, const std::string& uname, const std::string& pass)
    : userId(id), username(uname), password(pass) {}

int User::getUserId() const     { return userId; }
std::string User::getUsername() const { return username; }
std::string User::getPassword() const { return password; }

const std::vector<int>& User::getFriendIds() const { return friendIds; }

void User::addFriend(int friendUserId) {
    if (!hasFriend(friendUserId)) {
        friendIds.push_back(friendUserId);
    }
}

bool User::hasFriend(int otherId) const {
    return std::find(friendIds.begin(), friendIds.end(), otherId) != friendIds.end();
}

void User::printBasicInfo() const {
    std::cout << "ID: " << userId
              << " | Username: " << username
              << " | Friends count: " << friendIds.size() << "\n";
}