// Fixed LikeManager.h (include full Like.h instead of forward decl)
#ifndef LIKEMANAGER_H
#define LIKEMANAGER_H

#include <vector>
#include <string>

#include "GenericManager.h"
#include "Like.h"  // Full include instead of forward decl

class User;
class Post;
class PostManager;
class UserManager;
class AuthenticationService;
class LikeManager : public GenericManager<Like> {
public:
    LikeManager() = default;
    ~LikeManager();

    void addLike(Like* like);
    Like* createAndAdd(User* user, Post* post);

    // Hard delete one like (unlike)
    bool removeLike(User* user, Post* post);

    // Hard-delete all likes for a post (used when deleting a post)
    void removeLikesForPost(Post* post);

    Like* findLikeById(int id) const;
    const std::vector<Like*>& getAllLikes() const { return getItems(); }

    void clear();

    // Persistence
    // Format: likeId postId userId createdAt
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename,
                      PostManager& postManager,
                      UserManager& userManager);
    void loadFromFile(const std::string& filename,
                  PostManager& postManager,
                  AuthenticationService& authService);
};

#endif