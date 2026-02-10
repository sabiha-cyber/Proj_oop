#ifndef PAGE_H
#define PAGE_H

#include <string>
#include <vector>

class User;
class Post;

class Page
{
private:
    std::string pageName;
    std::vector<User*> followers;
    std::vector<Post*> posts;

public:
    Page(const std::string& name);

    
    bool addFollower(User* user);
    void addPost(Post* post);

    
    void showMenu(std::vector<User*>& users);

    
    void showPageInfo() const;
    void showTimeline() const;
    void showLatestPost() const;
};

#endif
