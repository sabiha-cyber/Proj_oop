#ifndef COMMENT_H
#define COMMENT_H

#include <string>
#include <cstddef>
#include <ctime>

class User;
class Post;

class Comment {
private:
    static int nextId;

    int commentId;
    User* author;
    Post* post;

    std::string text;
    size_t maxCapacity;
    bool isDeleted;
    time_t createdAt;

public:
    // Existing constructor (normal creation: auto ID + auto time)
    Comment(User* user, Post* postPtr, const std::string& content, size_t capacity);

    //constructor (loading: fixed ID + fixed time + deleted state)
    Comment(int id, User* user, Post* postPtr,
            const std::string& content, size_t capacity,
            time_t created, bool deleted);

    static Comment* createComment(User* user, Post* postPtr, size_t capacity);

    void editComment(const std::string& newText);
    void deleteComment();
    void viewComment() const;

    // Getters
    int getCommentId() const;
    User* getAuthor() const;
    Post* getPost() const;
    std::string getText() const;
    time_t getCreationTime() const;
    bool isDeletedComment() const;
    size_t getMaxCapacity() const;
};

#endif



