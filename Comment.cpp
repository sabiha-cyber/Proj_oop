#include "Comment.h"
#include "User.h"
#include "Post.h"
#include <limits>

using namespace std;

// Initialize static member
int Comment::nextId = 1;

// Constructor
Comment::Comment(User* user, Post* postPtr, const string& content, size_t capacity)
    : commentId(nextId++), author(user), post(postPtr), maxCapacity(capacity),
      isDeleted(false), createdAt(time(nullptr))
{
    if (content.length() <= maxCapacity) {
        text = content;
    } else {
        text = content.substr(0, maxCapacity);
        cout << "Warning: Comment text truncated to " << maxCapacity << " characters." << endl;
    }
}

// Factory-style creation
Comment* Comment::createComment(User* user, Post* postPtr, size_t capacity) {
    string content;
    cout << "Enter comment text (max " << capacity << " characters):" << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear buffer
    getline(cin, content);

    if (content.length() > capacity) {
        cout << "Warning: Comment text truncated to " << capacity << " characters." << endl;
        content = content.substr(0, capacity);
    }

    Comment* newComment = new Comment(user, postPtr, content, capacity);
    postPtr->addComment(newComment); // automatically attach to post
    cout << "Comment added successfully with ID " << newComment->getCommentId() << "!" << endl;
    return newComment;
}

// Edit comment
void Comment::editComment(const string& newText) {
    if (isDeleted) {
        cout << "Cannot edit a deleted comment." << endl;
        return;
    }

    if (newText.length() <= maxCapacity) {
        text = newText;
        cout << "Comment updated." << endl;
    } else {
        cout << "Error: New text exceeds maximum capacity of " << maxCapacity << " characters." << endl;
    }
}

// Delete comment
void Comment::deleteComment() {
    if (isDeleted) {
        cout << "Comment already deleted." << endl;
        return;
    }

    isDeleted = true;
    text = "[This comment has been deleted]";
    cout << "Comment " << commentId << " deleted." << endl;
}

// Display comment
void Comment::viewComment() const {
    if (isDeleted) {
        cout << "This comment has been deleted." << endl;
        return;
    }

    tm* timeinfo = localtime(&createdAt);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    cout << left << setw(15) << author->getUsername() << ": "
         << text << " (" << buffer << ")" << endl;
}

// Getters
int Comment::getCommentId() const { return commentId; }
User* Comment::getAuthor() const { return author; }
Post* Comment::getPost() const { return post; }
string Comment::getText() const { return text; }
time_t Comment::getCreationTime() const { return createdAt; }
bool Comment::isDeletedComment() const { return isDeleted; }
size_t Comment::getMaxCapacity() const { return maxCapacity; }


