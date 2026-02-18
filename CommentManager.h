#ifndef COMMENTMANAGER_H
#define COMMENTMANAGER_H

#include <vector>
#include <string>

class Comment;
class Post;
class PostManager;
class UserManager;

class CommentManager {
private:
    std::vector<Comment*> comments; // owns these pointers

public:
    CommentManager() = default;
    ~CommentManager();

    void addComment(Comment* comment);

    // When a post is deleted (soft deleted), Post clears its own comment list.
    // Then can safely delete those Comment objects here.
    void removeCommentsForPost(Post* post);

    // Delete ONE comment safely (soft delete: keeps pointer valid for Post::comments) (only replaced by text)
    bool deleteCommentById(int id);

    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename,
                      PostManager& postManager,
                      UserManager& userManager);

    Comment* findCommentById(int id) const;
    const std::vector<Comment*>& getAllComments() const;

    void clear();
};

#endif
