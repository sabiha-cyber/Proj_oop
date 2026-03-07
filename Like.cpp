#include "Like.h"
#include "User.h"
#include "Post.h"
#include "NotificationManager.h"
#include <iostream>
#include <ctime>

using namespace std;

int Like::nextId = 1;

// Normal constructor
Like::Like(User* userPtr, Post* postPtr)
    : likeId(nextId++), user(userPtr), post(postPtr), createdAt(time(nullptr)) {}

// Loading constructor
Like::Like(int id, User* userPtr, Post* postPtr, time_t created)
    : likeId(id), user(userPtr), post(postPtr), createdAt(created)
{
    if (id >= nextId) {
        nextId = id + 1;
    }
}

// Factory-style creation with double-like prevention
Like* Like::createLike(User* userPtr, Post* postPtr,NotificationManager* notifMgr) {
    if (!userPtr || !postPtr) return nullptr;

    for (Like* l : postPtr->getLikes()) {
        if (l && l->getUser() == userPtr) {
            cout << userPtr->getUsername() << " has already liked post "
                 << postPtr->getPostId() << endl;
            return nullptr;
        }
    }

    Like* newLike = new Like(userPtr, postPtr);
    postPtr->addLike(newLike);

    cout << userPtr->getUsername() << " liked post " << postPtr->getPostId()
         << " (Like ID: " << newLike->getLikeId() << ")" << endl;
    if (notifMgr && postPtr->getAuthor() && postPtr->getAuthor() != userPtr) {
        notifMgr->notifyLike(
            postPtr->getAuthor()->getUserId(),  // recipient  — real User*
            userPtr->getUserId(),               // actor      — real User*
            userPtr->getUsername(),             // from real User object
            postPtr->getPostId()                // from real Post object
        );
    }
    return newLike;
}

// Getters
int Like::getLikeId() const { return likeId; }
User* Like::getUser() const { return user; }
Post* Like::getPost() const { return post; }
time_t Like::getTime() const { return createdAt; }

// Display
void Like::viewLike() const {
    if (!user) {
        cout << "Unknown user liked this (Like ID: " << likeId << ")\n";
        return;
    }

    tm* timeinfo = localtime(&createdAt);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    cout << user->getUsername() << " liked this post at " << buffer
         << " (Like ID: " << likeId << ")" << endl;
}



