//user.h
#ifndef USER_H
#define USER_H

#include <string>
#include <vector>

class User {
private:
    int userId;
    std::string username;
    std::string password;
    std::vector<int> friendIds;           

public:
    User();
    User(int id, const std::string& uname, const std::string& pass);

  
    int                 getUserId()     const;
    std::string         getUsername()   const;
    std::string         getPassword()   const;

    
    const std::vector<int>& getFriendIds() const;

   
    void addFriend(int friendUserId);
    bool hasFriend(int otherId) const;

   
    void printBasicInfo() const;
};

#endif 

//user.cpp
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