// Fixed Post.h (remove inline body for deletePost)
#ifndef POST_H
#define POST_H

#include <string>
#include <vector>
#include <ctime>
#include <iosfwd>

#include "ContentItem.h"  // For inheritance

using namespace std;

// Forward declarations
class User;
class Like;
class Comment;

class Post : public ContentItem {
private:
    int postId;
    const size_t maxCapacity;

    vector<Like*> likes;
    vector<Comment*> comments;
    int shareCount;

    string category;
public:
    // Constructor
    Post(int id, User* user, const string& content, size_t capacity = 500, const string& cat = "General");

    // Factory-style creation
    static Post* createPost(int id, User* user, size_t capacity,const string& defaultCat = "General");

    // Post lifecycle
    void edit(const string& newText) override;
    void deletePost();  // Declaration only, no body

    // View
    void display()const override;
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
    void removeCommentPointer(Comment* c);

    // Share
    void sharePost();

    // Getters
    int getPostId() const;
    User* getAuthor() const { return author; }  // From base if needed
    string getText() const { return text; }  // From base
    int getShareCount() const;
    size_t getMaxCapacity() const;
    bool isDeletedPost() const { return isDeletedItem(); }  // Wrapper to base, keep inline
    time_t getCreationTime() const { return createdAt; }  // From base

    // Category management
    string getCategory() const;          // getter
    void setCategory(const string& cat); // setter

    //Friend Function
    friend std::ostream& operator<<(std::ostream& os, const Post& p);
};

#endif