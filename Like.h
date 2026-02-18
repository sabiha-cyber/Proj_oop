#ifndef LIKE_H
#define LIKE_H

#include <ctime>
#include <iostream>

using namespace std;

class User; // forward declaration
class Post; // forward declaration

class Like {
private:
    static int nextId;   // auto-increment ID
    int likeId;          // unique like ID
    User* user;          // who liked
    Post* post;          // post that was liked
    time_t createdAt;    // timestamp

public:
    // Constructor
    Like(User* userPtr, Post* postPtr);

    // Factory-style creation
    static Like* createLike(User* userPtr, Post* postPtr);

    // Getters
    int getLikeId() const;
    User* getUser() const;
    Post* getPost() const;
    time_t getTime() const;

    // Display
    void viewLike() const;
};

#endif


