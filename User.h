
#ifndef USER_H
#define USER_H

#include <string>
#include <vector>

class Post;  

class User {
private:
    int userId;
    std::string username;
    std::string password;
    std::vector<int> friendIds;
    std::vector<Post*> myPosts;          

public:
    User();
    User(int id, const std::string& uname, const std::string& pass);

    int                 getUserId()     const;
    std::string         getUsername()   const;
    std::string         getPassword()   const;

    const std::vector<int>& getFriendIds() const;

    void addFriend(int friendUserId);
    bool hasFriend(int otherId) const;

    void printBasicInfo() const;

  
    void                addPost(Post* post);
    const std::vector<Post*>& getPosts() const;
    void                showMyPosts() const;
};

#endif
