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
    // Create new page — admin is the current logged-in User*
    Page(const std::string& name, const std::string& desc,
         const std::string& cat, User* admin);

    // Load from file — fixed id and time
    Page(int id, const std::string& name, const std::string& desc,
         const std::string& cat, User* admin, time_t createdAt);

    ~Page();

    // ── Follow ────────────────────────────────────────────────────────────────
    bool followPage  (User* user);
    bool unfollowPage(User* user);
    bool isFollower  (User* user) const;
    int  followerCount()          const;
    void showFollowers()          const;

    // ── Posts — author is the current logged-in User* ─────────────────────────
    Post* createPost(User* author, const std::string& content,
                     const std::string& cat = "General");
    void  attachPost(Post* post);   // used when loading from file
    void  deletePost(int postId);
    Post* findPost  (int postId) const;

    // Like / comment on a post — delegates straight to Post/Like/Comment
    void likePost     (int postId, User* user);
    void commentOnPost(int postId, User* user, const std::string& text);

    // ── Display ───────────────────────────────────────────────────────────────
    void showPageInfo()   const;
    void showTimeline()   const;
    void showLatestPost() const;

    // ── Getters ───────────────────────────────────────────────────────────────
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
