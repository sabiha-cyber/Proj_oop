#ifndef PAGE_H
#define PAGE_H

#include <string>
#include <vector>
#include <ctime>

class User;
class Post;
class AuthenticationService;

class Page {
private:
    static int nextPageId;

    int         pageId;
    std::string pageName;
    std::string description;
    std::string category;
    User*       admin;       // current logged-in user who created the page
    time_t      createdAt;

    std::vector<User*> followers;
    std::vector<Post*> posts;   // page owns these posts

public:
  
    Page(const std::string& name, const std::string& desc,
         const std::string& cat, User* admin);

   
    Page(int id, const std::string& name, const std::string& desc,
         const std::string& cat, User* admin, time_t createdAt);

    ~Page();

   
    bool followPage  (User* user);
    bool unfollowPage(User* user);
    bool isFollower  (User* user) const;
    int  followerCount()          const;
    void showFollowers()          const;

    
    Post* createPost(User* author, const std::string& content,
                     const std::string& cat = "General");
    void  attachPost(Post* post);   // used when loading from file
    void  deletePost(int postId);
    Post* findPost  (int postId) const;

   
    void likePost     (int postId, User* user);
    void commentOnPost(int postId, User* user, const std::string& text);

    
    void showPageInfo()   const;
    void showTimeline()   const;
    void showLatestPost() const;

   
    int         getPageId()      const;
    std::string getPageName()    const;
    std::string getDescription() const;
    std::string getCategory()    const;
    User*       getAdmin()       const;
    time_t      getCreatedAt()   const;
    const std::vector<User*>& getFollowers() const;
    const std::vector<Post*>& getPosts()     const;

    void setDescription(const std::string& d);
    void setCategory   (const std::string& c);
};

#endif
