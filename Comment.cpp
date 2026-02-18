#include "Comment.h"
#include "User.h"
#include "Post.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <ctime>
#include <string>

using namespace std;

// Static member initialization
int Comment::nextId = 1;

// Constructor
Comment::Comment(User* user, Post* postPtr, const string& content, size_t capacity)
    : commentId(nextId++),
      author(user),
      post(postPtr),
      maxCapacity(capacity),
      isDeleted(false),
      createdAt(time(nullptr))
{
    if (content.length() <= maxCapacity) {
        text = content;
    } else {
        text = content.substr(0, maxCapacity);
        cout << "Warning: Comment truncated to " << maxCapacity << " characters.\n";
    }
}

// Interactive creation
Comment* Comment::createComment(User* user, Post* postPtr, size_t capacity) {
    if (!user || !postPtr) {
        cout << "Error: Invalid user or post.\n";
        return nullptr;
    }

    string content;

    cout << "\n--- Add Comment ---\n";
    cout << "Enter comment (max " << capacity << " characters):\n";

    // Clear leftover newline from menu choice
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, content);

    // Basic trim
    size_t first = content.find_first_not_of(" \t\n\r\f\v");
    if (first == string::npos) {
        cout << "Comment cannot be empty. Cancelled.\n";
        return nullptr;
    }
    size_t last = content.find_last_not_of(" \t\n\r\f\v");
    content = content.substr(first, (last - first + 1));

    if (content.length() > capacity) {
        cout << "Warning: Comment truncated to " << capacity << " characters.\n";
        content = content.substr(0, capacity);
    }

    Comment* newComment = new Comment(user, postPtr, content, capacity);
    postPtr->addComment(newComment);
    cout << "Comment added successfully! (ID: " << newComment->getCommentId() << ")\n";

    return newComment;
}

// Edit comment
void Comment::editComment(const string& newText) {
    if (isDeleted) {
        cout << "Cannot edit a deleted comment.\n";
        return;
    }

    if (newText.length() <= maxCapacity) {
        text = newText;
        cout << "Comment updated.\n";
    } else {
        cout << "Error: Text exceeds max capacity (" << maxCapacity << " characters).\n";
    }
}

//Deletion
void Comment::deleteComment() {
    if (isDeleted) {
        cout << "Comment already deleted.\n";
        return;
    }

    isDeleted = true;
    text = "[This comment has been deleted]";
    cout << "Comment #" << commentId << " deleted.\n";
}

// Display comment
void Comment::viewComment() const {
    if (isDeleted) {
        cout << "[Deleted comment]\n";
        return;
    }

    if (!author) {
        cout << "Unknown user: " << text << "\n";
        return;
    }

    tm* timeinfo = localtime(&createdAt);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    cout << left << setw(15) << author->getUsername() << ": "
         << text << "  (" << buffer << ")\n";
}

// Getters
int     Comment::getCommentId()     const { return commentId; }
User*   Comment::getAuthor()        const { return author; }
Post*   Comment::getPost()          const { return post; }
string  Comment::getText()          const { return text; }
time_t  Comment::getCreationTime()  const { return createdAt; }
bool    Comment::isDeletedComment() const { return isDeleted; }
size_t  Comment::getMaxCapacity()   const { return maxCapacity; }


