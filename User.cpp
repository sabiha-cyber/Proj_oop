#include "User.h"
#include "Post.h"           // needed for Post*

#include <iostream>
#include <algorithm>
#include <vector>

User::User() : userId(-1) {}

User::User(int id, const std::string& uname, const std::string& pass)
    : userId(id), username(uname), password(pass) {}

int User::getUserId() const {
    return userId;
}

std::string User::getUsername() const {
    return username;
}

std::string User::getPassword() const {
    return password;
}

const std::vector<int>& User::getFriendIds() const {
    return friendIds;
}

void User::addFriend(int friendUserId) {
    if (!hasFriend(friendUserId)) {
        friendIds.push_back(friendUserId);
    }
}

bool User::hasFriend(int otherId) const {
    return std::find(friendIds.begin(), friendIds.end(), otherId) != friendIds.end();
}

// ─────────────────────────────────────────────────────────────────────────────
// NEW: Remove friend (unfriend)
// ─────────────────────────────────────────────────────────────────────────────
void User::removeFriend(int friendUserId) {
    auto it = std::find(friendIds.begin(), friendIds.end(), friendUserId);
    if (it != friendIds.end()) {
        friendIds.erase(it);
    }
    // Note: we don't print anything here — printing is handled in FriendService
}

void User::printBasicInfo() const {
    std::cout << "ID: " << userId
              << " | Username: " << username
              << " | Friends count: " << friendIds.size() << "\n";
}

void User::addPost(Post* post) {
    if (post != nullptr) {
        myPosts.push_back(post);
    }
}

const std::vector<Post*>& User::getPosts() const {
    return myPosts;
}

void User::showMyPosts() const {
    if (myPosts.empty()) {
        std::cout << "You haven't posted anything yet.\n";
        return;
    }

    std::cout << "\nPosts by @" << username << " (" << myPosts.size() << " total):\n";
    std::cout << "----------------------------------------\n";

    for (const Post* p : myPosts) {
        if (p) {
            p->viewPost();
            std::cout << "----------------------------------------\n";
        }
    }
}