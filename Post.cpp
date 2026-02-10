#include "Post.h"
#include "User.h"
#include "Like.h"
#include "Comment.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <ctime>
#include <string>

using namespace std;

// Constructor
Post::Post(int id, User* user, const string& content, size_t capacity, const string& cat)
    : postId(id),
      author(user),
      text(),
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
        cout << "Warning: Post content truncated to " << maxCapacity << " characters.\n";
    }
}

//Interactive creation
Post* Post::createPost(int id, User* user, size_t capacity, const string& defaultCat) {
    string content;
    string cat;

    cout << "\n--- Create New Post ---\n";

    // Clear any leftover input from previous menu choice
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Get content
    cout << "Enter post content (max " << capacity << " characters):\n";
    getline(cin, content);

    // Basic validation & trimming
    size_t first = content.find_first_not_of(" \t\n\r\f\v");
    if (first == string::npos) {
        cout << "Error: Post content cannot be empty. Creation cancelled.\n";
        return nullptr;
    }
    size_t last = content.find_last_not_of(" \t\n\r\f\v");
    content = content.substr(first, (last - first + 1));

    if (content.length() > capacity) {
        cout << "Warning: Content truncated to " << capacity << " characters.\n";
        content = content.substr(0, capacity);
    }

    // Get category
    cout << "\nEnter category (press Enter for default '" << defaultCat << "'): ";
    getline(cin, cat);

    // Trim category
    first = cat.find_first_not_of(" \t\n\r\f\v");
    if (first == string::npos) {
        cat = defaultCat;
    } else {
        last = cat.find_last_not_of(" \t\n\r\f\v");
        cat = cat.substr(first, (last - first + 1));
    }

    Post* newPost = new Post(id, user, content, capacity, cat);
    cout << "Post created successfully! (ID: " << id << ")\n";

    return newPost;
}


// Edit post
void Post::editPost(const string& newText) {
    if (isDeleted) {
        cout << "Cannot edit a deleted post.\n";
        return;
    }

    if (newText.length() <= maxCapacity) {
        text = newText;
        cout << "Post updated successfully.\n";
    } else {
        cout << "Error: New text exceeds maximum capacity ("
             << maxCapacity << " characters).\n";
    }
}

// Delete post
void Post::deletePost() {
    if (isDeleted) {
        cout << "Post is already deleted.\n";
        return;
    }

    isDeleted = true;
    text = "[This post has been deleted]";
    likes.clear();
    comments.clear();
    shareCount = 0;

    cout << "Post #" << postId << " has been deleted.\n";
}

// View post
void Post::viewPost() const {
    if (isDeleted) {
        cout << "This post has been deleted.\n";
        return;
    }

    cout << left << setw(14) << "Author:"
         << (author ? author->getUsername() : "Unknown") << "\n"
         << left << setw(14) << "Category:"   << category << "\n"
         << left << setw(14) << "Content:"    << text << "\n"
         << left << setw(14) << "Likes:"      << getLikeCount()
         << setw(14) << "Comments:" << comments.size()
         << setw(14) << "Shares:"   << shareCount << "\n";

    tm* timeinfo = localtime(&createdAt);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    cout << left << setw(14) << "Created:" << buffer << "\n";
}


// Like management
void Post::addLike(Like* like) {
    if (!isDeleted && like != nullptr) {
        likes.push_back(like);
    }
}

void Post::removeLike(Like* like) {
    if (isDeleted || like == nullptr) {
        return;
    }

    auto it = find(likes.begin(), likes.end(), like);
    if (it != likes.end()) {
        likes.erase(it);
    }
}

int Post::getLikeCount() const {
    return isDeleted ? 0 : static_cast<int>(likes.size());
}

const vector<Like*>& Post::getLikes() const {
    return likes;
}

void Post::viewLikes() const {
    if (isDeleted) {
        cout << "Cannot view likes – post is deleted.\n";
        return;
    }

    if (likes.empty()) {
        cout << "No likes yet.\n";
        return;
    }

    cout << "Likes (" << likes.size() << "):\n";
    for (const Like* l : likes) {
        if (l && l->getUser()) {
            cout << "  - " << l->getUser()->getUsername() << "\n";
        }
    }
}

// Comment management
void Post::addComment(Comment* comment) {
    if (!isDeleted && comment != nullptr) {
        comments.push_back(comment);
    }
}

void Post::viewComments() const {
    if (isDeleted) {
        cout << "Cannot view comments – post is deleted.\n";
        return;
    }

    if (comments.empty()) {
        cout << "No comments yet.\n";
        return;
    }

    cout << "Comments (" << comments.size() << "):\n";
    for (const Comment* c : comments) {
        if (c && c->getAuthor()) {
            cout << "  " << left << setw(15)
                 << c->getAuthor()->getUsername() << ": "
                 << c->getText() << "\n";
        }
    }
}

const vector<Comment*>& Post::getComments() const {
    return comments;
}

// Share
void Post::sharePost() {
    if (!isDeleted) {
        ++shareCount;
    }
}

// Getters & setters
int    Post::getPostId()       const { return postId; }
User*  Post::getAuthor()       const { return author; }
string Post::getText()         const { return text; }
int    Post::getShareCount()   const { return shareCount; }
size_t Post::getMaxCapacity()  const { return maxCapacity; }
bool   Post::isDeletedPost()   const { return isDeleted; }
time_t Post::getCreationTime() const { return createdAt; }
string Post::getCategory()     const { return category; }
void   Post::setCategory(const string& cat) { category = cat; }



