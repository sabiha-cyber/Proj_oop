#ifndef COMMENT_H
#define COMMENT_H

#include <string>
#include <ctime>
#include <iostream>

using namespace std;

class User; // forward declaration
class Post; // forward declaration

class Comment {
private:
    static int nextId;   // static variable for auto-increment
    int commentId;       // unique comment ID
    User* author;        // pointer to author
    Post* post;          // pointer to the post this comment belongs to
    string text;         // comment text
    const size_t maxCapacity; // max allowed characters
    time_t createdAt;    // timestamp
    bool isDeleted;      // deleted flag

public:
    // Constructor
    Comment(User* user, Post* postPtr, const string& content, size_t capacity = 200);

    //Interactive creation
    static Comment* createComment(User* user, Post* postPtr, size_t capacity = 200);

    // Lifecycle
    void editComment(const string& newText);
    void deleteComment();

    // Display
    void viewComment() const;

    // Getters
    int getCommentId() const;
    User* getAuthor() const;
    Post* getPost() const;
    string getText() const;
    time_t getCreationTime() const;
    bool isDeletedComment() const;
    size_t getMaxCapacity() const;
};

#endif



