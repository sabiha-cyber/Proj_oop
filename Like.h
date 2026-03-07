#ifndef LIKE_H
#define LIKE_H

#include <ctime>

class User;
class Post;
class NotificationManager;

class Like {
private:
    static int nextId;
    int likeId;
    User* user;
    Post* post;
    time_t createdAt;

public:
    // Normal constructor (auto ID + current time)
    Like(User* userPtr, Post* postPtr);

    // Loading constructor (fixed ID + fixed time)
    Like(int id, User* userPtr, Post* postPtr, time_t created);

    // Factory-style creation (prevents double-like)
    static Like* createLike(User* userPtr, Post* postPtr, NotificationManager* notifMgr = nullptr);

    // Getters
    int getLikeId() const;
    User* getUser() const;
    Post* getPost() const;
    time_t getTime() const;

    // Display
    void viewLike() const;
};

#endif


