#include "Post.h"
#include "User.h"
#include "Like.h"
#include "Comment.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

// Constructor
Post::Post(int id, User* user, const string& content, size_t capacity, const string& cat)
    : postId(id),
      author(user),
      maxCapacity(capacity),
      isDeleted(false),
      shareCount(0),
      createdAt(time(nullptr)),
      category(cat)
{
    if (content.length() <= maxCapacity) {
        text = content;
    } else {
        text = content.substr(0, maxCapacity);
        cout << "Warning: Post content truncated to " << maxCapacity << " characters." << endl;
    }
}

// Factory-style creation
Post* Post::createPost(int id, User* user, size_t capacity) {
    string content;
    string cat;

    // Get post content
    cout << "Enter post content (max " << capacity << " characters):" << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear leftover input
    getline(cin, content);

    if (content.length() > capacity) {
        cout << "Warning: Post truncated to " << capacity << " characters." << endl;
        content = content.substr(0, capacity);
    }

    // Get post category
    cout << "Enter post category (e.g., News, Entertainment, Sports):" << endl;
    getline(cin, cat);

    Post* newPost = new Post(id, user, content, capacity, cat); // pass category
    cout << "Post created successfully!" << endl;
    return newPost;
}


// Edit post
void Post::editPost(const string& newText) {
    if (isDeleted) {
        cout << "Cannot edit a deleted post." << endl;
        return;
    }

    if (newText.length() <= maxCapacity) {
        text = newText;
        cout << "Post updated." << endl;
    } else {
        cout << "Error: New text exceeds maximum capacity of " << maxCapacity << " characters." << endl;
    }
}

// Delete post
void Post::deletePost() {
    if (isDeleted) {
        cout << "Post already deleted." << endl;
        return;
    }

    isDeleted = true;
    text = "[This post has been deleted]";
    likes.clear();
    comments.clear();
    shareCount = 0;

    cout << "Post " << postId << " deleted." << endl;
}

// View post
void Post::viewPost() const {
    if (isDeleted) {
        cout << "This post has been deleted." << endl;
        return;
    }

    cout << left << setw(14) << "Author:" << author->getUsername() << "\n"
         << left << setw(14) << "Category:" << category << "\n"
         << left << setw(14) << "Content:" << text << "\n"
         << left << setw(14) << "Likes:" << getLikeCount()
         << setw(14) << "Comments:" << comments.size()
         << setw(14) << "Shares:" << shareCount << "\n";

    tm* timeinfo = localtime(&createdAt);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    cout << left << setw(14) << "Created:" << buffer << endl;
}

// Likes
void Post::addLike(Like* like) {
    if (!isDeleted) likes.push_back(like);
}

void Post::removeLike(Like* like) {
    if (isDeleted) return;

    auto it = find(likes.begin(), likes.end(), like);
    if (it != likes.end()) likes.erase(it);
}

int Post::getLikeCount() const {
    return isDeleted ? 0 : static_cast<int>(likes.size());
}

void Post::viewLikes() const {
    if (isDeleted) {
        cout << "Cannot view likes of a deleted post." << endl;
        return;
    }

    if (likes.empty()) {
        cout << "No likes yet." << endl;
        return;
    }

    cout << "Likes (" << likes.size() << "):\n";
    for (const Like* l : likes) {
        cout << "  - " << l->getUser()->getUsername() << endl;
    }
}

// Comments
void Post::addComment(Comment* comment) {
    if (!isDeleted) comments.push_back(comment);
}

void Post::viewComments() const {
    if (isDeleted) {
        cout << "Cannot view comments of a deleted post." << endl;
        return;
    }

    if (comments.empty()) {
        cout << "No comments yet." << endl;
        return;
    }

    cout << "Comments (" << comments.size() << "):\n";
    for (const Comment* c : comments) {
        cout << "  " << left << setw(15)
             << c->getAuthor()->getUsername() << ": "
             << c->getText() << "\n";
    }
}

const vector<Comment*>& Post::getComments() const {
    return comments;
}

// Share
void Post::sharePost() {
    if (!isDeleted) ++shareCount;
}

// Getters
int Post::getPostId() const { return postId; }
User* Post::getAuthor() const { return author; }
string Post::getText() const { return text; }
int Post::getShareCount() const { return shareCount; }
size_t Post::getMaxCapacity() const { return maxCapacity; }
bool Post::isDeletedPost() const { return isDeleted; }
time_t Post::getCreationTime() const { return createdAt; }

//Category added
string Post::getCategory() const { return category; }
void Post::setCategory(const string& cat) { category = cat; }



