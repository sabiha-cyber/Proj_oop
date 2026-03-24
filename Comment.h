#ifndef COMMENT_H
#define COMMENT_H

#include <string>
#include <cstddef>
#include <ctime>

#include "ContentItem.h"

class User;
class Post;
class NotificationManager;

class Comment : public ContentItem {
private:
    static int nextId;

    int commentId;
    Post* post;
    size_t maxCapacity;

public:
    Comment(User* user, Post* postPtr, const std::string& content, size_t capacity, NotificationManager* notifMgr = nullptr);
    Comment(int id, User* user, Post* postPtr,
            const std::string& content, size_t capacity,
            time_t created, bool deleted);

    static Comment* createComment(User* user, Post* postPtr, size_t capacity, NotificationManager* notifMgr = nullptr);

    void edit(const std::string& newText) override;
    void deleteComment();
    void display() const override;

    int getCommentId() const;
    User* getAuthor() const { return author; }
    Post* getPost() const;
    std::string getText() const { return text; }
    time_t getCreationTime() const { return createdAt; }
    bool isDeletedComment() const { return isDeletedItem(); }
    size_t getMaxCapacity() const;

    void viewComment() const;  // Ensure const here
};

#endif