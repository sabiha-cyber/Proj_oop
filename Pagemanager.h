#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <vector>
#include <string>

class Page;
class User;
class AuthenticationService;

class PageManager {
private:
    std::vector<Page*> pages;

public:
    static const std::string PAGES_FILE;

    PageManager();
    ~PageManager();

    // admin = current logged-in User*
    Page* createPage(const std::string& name, const std::string& desc,
                     const std::string& cat, User* admin);
    bool  deletePage(int pageId, User* requestingUser);

    Page*              findById     (int pageId)              const;
    Page*              findByName   (const std::string& name) const;
    std::vector<Page*> getByAdmin   (User* admin)             const;
    std::vector<Page*> getByCategory(const std::string& cat)  const;
    std::vector<Page*> getAllPages  ()                         const;

    bool followPage  (User* user, int pageId);
    bool unfollowPage(User* user, int pageId);

    void likePost     (int pageId, int postId, User* user);
    void commentOnPost(int pageId, int postId, User* user, const std::string& text);

    void listAllPages() const;

    // currentUser = logged-in User* — all menu actions use this
    void showPageMenu(int pageId, User* currentUser);

    void saveToFile  (const std::string& filename = PAGES_FILE) const;
    void loadFromFile(const std::string& filename, AuthenticationService& auth);
};

#endif