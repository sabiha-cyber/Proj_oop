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
#include <ostream>
using namespace std;

// Constructor
Post::Post(int id, User* user, const string& content, size_t capacity, const string& cat)
    : ContentItem(user, content.length() <= capacity ? content : content.substr(0, capacity)),
      postId(id), maxCapacity(capacity), shareCount(0), category(cat)
{
    if (content.length() > capacity) {
        cout << "Warning: Post content truncated to " << capacity << " characters.\n";
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
void Post::edit(const string& newText) {
    if (isDeletedItem()) {
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

// Display override
void Post::display() const {
    viewPost();  // Reuse
}

// Delete (call base)
void Post::deletePost() {
    deleteItem();
    likes.clear();
    comments.clear();
    shareCount = 0;
    cout << "Post #" << postId << " has been deleted.\n";
}

// View post
std::ostream& operator<<(std::ostream& os, const Post& p){
    if (p.isDeletedItem()) {
        os << "This post has been deleted.\n";
        return os;
    }

    os << std::left << std::setw(14) << "Author:"
       << (p.getAuthor() ? p.getAuthor()->getUsername() : "Unknown") << "\n"
       << std::left << std::setw(14) << "Category:" << p.getCategory() << "\n"
       << std::left << std::setw(14) << "Content:" << p.getText() << "\n"
       << std::left << std::setw(14) << "Likes:" << p.getLikeCount()
       << std::setw(14) << "Comments:" << p.getComments().size()
       << std::setw(14) << "Shares:" << p.getShareCount() << "\n";

    std::time_t t = p.getCreationTime();
    std::tm* timeinfo = std::localtime(&t);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    os << std::left << std::setw(14) << "Created:" << buffer << "\n";

    return os;
}
void Post::viewPost() const {
    std::cout << *this;
}
// Like management
void Post::addLike(Like* like) {
    if (!isDeletedItem() && like != nullptr) {
        likes.push_back(like);
    }
}
void Post::removeLike(Like* like) {
    if (isDeletedItem() || like == nullptr) {
        return;
    }
    auto it = find(likes.begin(), likes.end(), like);
    if (it != likes.end()) {
        likes.erase(it);
    }
}
int Post::getLikeCount() const {
    return isDeletedItem() ? 0 : static_cast<int>(likes.size());
}
const vector<Like*>& Post::getLikes() const {
    return likes;
}
void Post::viewLikes() const {
    if (isDeletedItem()) {
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
            cout << " - " << l->getUser()->getUsername() << "\n";
        }
    }
}
// Comment management
void Post::addComment(Comment* comment) {
    if (!isDeletedItem() && comment != nullptr) {
        comments.push_back(comment);
    }
}
void Post::viewComments() const {
    if (isDeletedItem()) {
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
            cout << " " << left << setw(15)
                 << c->getAuthor()->getUsername() << ": "
                 << c->getText() << "\n";
        }
    }
}
const vector<Comment*>& Post::getComments() const {
    return comments;
}
void Post::removeCommentPointer(Comment* c) {
    if (!c) return;
    auto it = std::find(comments.begin(), comments.end(), c);
    if (it != comments.end()) {
        comments.erase(it);
    }
}
// Share
void Post::sharePost() {
    if (!isDeletedItem()) {
        ++shareCount;
    }
}
// Getters & setters
int Post::getPostId() const { return postId; }
int Post::getShareCount() const { return shareCount; }
size_t Post::getMaxCapacity() const { return maxCapacity; }
string Post::getCategory() const { return category; }
void Post::setCategory(const string& cat) { category = cat; }