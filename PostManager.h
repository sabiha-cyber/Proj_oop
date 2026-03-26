#ifndef POSTMANAGER_H
#define POSTMANAGER_H

#include <vector>
#include <string>
#include "GenericManager.h"  // Include the template header

using namespace std;

class Post;
class User;
class LikeManager;
class CommentManager;

class PostManager : public GenericManager<Post> {  // Template usage
private:
    size_t  defaultCapacity;
    int  nextPostId;

public:
    explicit PostManager(size_t capacity = 500);
    ~PostManager();

    // Core
    void  addPost(Post* p);
    Post* createAndAdd(User* user);
    Post* createAndAdd(User* user, const string& content,
                       const string& cat = "General");

    // Lookup
    Post* findPostById(int id)const;
    vector<Post*> getPostsByUser(User* user)const;
    vector<Post*> getPostsByCategory(const string& cat) const;
    vector<Post*> getAllActivePosts() const;

    // Deletion (soft - post* stays in vector)
    void deletePostById(int id, User* requestingUser,
                        CommentManager& cm, LikeManager& lm);

    // Persistence
    void saveToFile(const string& filename)const;
    void loadFromFile(const string& filename, vector<User*>& users);

    // Getters
    int getNextPostId() const;
    const vector<Post*>& getPosts() const { return getItems(); }  // Wrapper
};

#endif