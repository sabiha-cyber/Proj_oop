#include "Comment.h"
#include "User.h"
#include "Post.h"
#include "NotificationManager.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <ctime>

int Comment::nextId = 1;

Comment::Comment(User* user, Post* postPtr,
                 const std::string& content, size_t capacity,
                 NotificationManager* notifMgr)
    : ContentItem(user, (content.length() <= capacity) ? content : content.substr(0, capacity)),
      commentId(nextId++),
      post(postPtr),
      maxCapacity(capacity)
{
    if (content.length() > capacity) {
        std::cout << "Warning: Comment truncated to " << capacity << " characters.\n";
    }

    // Notify post author (if different from commenter)
    if (notifMgr && user && postPtr && postPtr->getAuthor() && postPtr->getAuthor() != user) {
        notifMgr->notifyComment(
            postPtr->getAuthor()->getUserId(),
            user->getUserId(),
            user->getUsername(),
            postPtr->getPostId(),
            text.substr(0, 60)
        );
    }
}

Comment::Comment(int id, User* user, Post* postPtr,
                 const std::string& content, size_t capacity,
                 time_t created, bool deleted)
    : ContentItem(user, (content.length() <= capacity) ? content : content.substr(0, capacity)),
      commentId(id),
      post(postPtr),
      maxCapacity(capacity)
{
    if (id >= nextId) nextId = id + 1;

    createdAt = created;
    isDeleted = deleted;
}

Comment* Comment::createComment(User* user, Post* postPtr, size_t capacity, NotificationManager* notifMgr)
{
    if (!user || !postPtr) return nullptr;

    std::cout << "Enter comment text (max " << capacity << " characters): ";

    // In case previous reads left a newline in the buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string content;
    std::getline(std::cin, content);

    return new Comment(user, postPtr, content, capacity, notifMgr);
}

void Comment::edit(const std::string& newText)
{
    if (isDeletedItem()) {
        std::cout << "Cannot edit a deleted comment.\n";
        return;
    }

    if (newText.length() <= maxCapacity) {
        text = newText;
        std::cout << "Comment updated.\n";
    } else {
        std::cout << "Error: Text exceeds max capacity (" << maxCapacity << " characters).\n";
    }
}

void Comment::display() const
{
    viewComment();
}

void Comment::deleteComment()
{
    deleteItem();
    std::cout << "Comment #" << commentId << " deleted.\n";
}

void Comment::viewComment() const
{
    if (isDeletedItem()) {
        std::cout << "[Deleted comment]\n";
        return;
    }

    if (!author) {
        std::cout << "Unknown user: " << text << "\n";
        return;
    }

    std::tm* timeinfo = std::localtime(&createdAt);
    char buffer[64] = "Unknown time";

    if (timeinfo) {
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    }

    std::cout << std::left << std::setw(15) << author->getUsername()
              << ": " << text << "  (" << buffer << ")\n";
}

int Comment::getCommentId() const { return commentId; }
Post* Comment::getPost() const { return post; }
size_t Comment::getMaxCapacity() const { return maxCapacity; }