#include "Like.h"
#include "User.h"
#include "Post.h"

#include <iostream>
using namespace std;

// Initialize static member
int Like::nextId = 1;

// Constructor
Like::Like(User* userPtr, Post* postPtr)
    : likeId(nextId++), user(userPtr), post(postPtr), createdAt(time(nullptr)) {}

// Factory-style creation with double-like prevention
Like* Like::createLike(User* userPtr, Post* postPtr) {
    // Check if this user already liked the post
    for (Like* l : postPtr->getLikes()) {
        if (l->getUser() == userPtr) {
            cout << userPtr->getUsername() << " has already liked post "
                 << postPtr->getPostId() << endl;
            return nullptr;  // Do not create a new like
        }
    }

    // If not liked yet, create and attach
    Like* newLike = new Like(userPtr, postPtr);
    postPtr->addLike(newLike);
    cout << userPtr->getUsername() << " liked post " << postPtr->getPostId()
         << " (Like ID: " << newLike->getLikeId() << ")" << endl;
    return newLike;
}

// Getters
int Like::getLikeId() const { return likeId; }
User* Like::getUser() const { return user; }
Post* Like::getPost() const { return post; }
time_t Like::getTime() const { return createdAt; }

// Display
void Like::viewLike() const {
    tm* timeinfo = localtime(&createdAt);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    cout << user->getUsername() << " liked this post at " << buffer
         << " (Like ID: " << likeId << ")" << endl;
}



