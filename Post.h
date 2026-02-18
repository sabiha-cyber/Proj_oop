#ifndef POST_H
#define POST_H

#include <string>
#include <vector>
#include <ctime>
#include <iosfwd>

using namespace std;

// Forward declarations
class User;
class Like;
class Comment;

class Post {
private:
    int postId;
    User* author;
    string text;
    const size_t maxCapacity;
    bool isDeleted;

    time_t createdAt;
    vector<Like*> likes;
    vector<Comment*> comments;
    int shareCount;

    string category;
public:
    // Constructor
    Post(int id, User* user, const string& content, size_t capacity = 500, const string& cat = "General");

    // Factory-style creation
    static Post* createPost(int id, User* user, size_t capacity = 500, const string& cat = "General");

    // Post lifecycle
    void editPost(const string& newText);
    void deletePost();

    // View
    void viewPost() const;
    void viewLikes() const;
    void viewComments() const;

    // Like management
    void addLike(Like* like);
    void removeLike(Like* like);
    int getLikeCount() const;
    const vector<Like*>& getLikes() const;//to prevent double liking


    // Comment management
    void addComment(Comment* comment);
    const vector<Comment*>& getComments() const;

    // Share
    void sharePost();

    // Getters
    int getPostId() const;
    User* getAuthor() const;
    string getText() const;
    int getShareCount() const;
    size_t getMaxCapacity() const;
    bool isDeletedPost() const;
    time_t getCreationTime() const;

    // Category management
    string getCategory() const;          // getter
    void setCategory(const string& cat); // setter
};

#endif




