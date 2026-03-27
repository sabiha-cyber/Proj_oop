#include "Page.h"
#include "User.h"
#include "Post.h"
#include "Like.h"
#include "Comment.h"

#include <iostream>
#include <algorithm>
#include <ctime>
using namespace std;

int Page::nextPageId = 1;



Page::Page(const string& name, const string& desc, const string& cat, User* adm)
    : pageId(nextPageId++), pageName(name), description(desc),
      category(cat), admin(adm), createdAt(time(nullptr)) {}

Page::Page(int id, const string& name, const string& desc,
           const string& cat, User* adm, time_t t)
    : pageId(id), pageName(name), description(desc),
      category(cat), admin(adm), createdAt(t)
{
    if (id >= nextPageId) nextPageId = id + 1;
}

Page::~Page() {
    for (Post* p : posts) delete p;
}



bool Page::followPage(User* user) {
    if (!user) return false;
    if (isFollower(user)) {
        cout << user->getUsername() << " already follows '" << pageName << "'\n";
        return false;
    }
    followers.push_back(user);
    cout << user->getUsername() << " followed '" << pageName << "'\n";
    return true;
}

bool Page::unfollowPage(User* user) {
    auto it = find(followers.begin(), followers.end(), user);
    if (it == followers.end()) {
        cout << user->getUsername() << " does not follow '" << pageName << "'\n";
        return false;
    }
    followers.erase(it);
    cout << user->getUsername() << " unfollowed '" << pageName << "'\n";
    return true;
}

bool Page::isFollower(User* user) const {
    return find(followers.begin(), followers.end(), user) != followers.end();
}

int  Page::followerCount() const { return (int)followers.size(); }

void Page::showFollowers() const {
    cout << "\n  Followers (" << followers.size() << "):\n";
    if (followers.empty()) { cout << "  None yet.\n"; return; }
    for (const User* u : followers)
        cout << "  - @" << u->getUsername()
             << " (ID: " << u->getUserId() << ")\n";
}



Post* Page::createPost(User* author, const string& content, const string& cat) {
    if (!author) { cout << "Error: no author.\n"; return nullptr; }
    Post* p = new Post((int)posts.size() + 1, author, content, 500, cat);
    posts.push_back(p);
    cout << "[" << pageName << "] @" << author->getUsername()
         << " posted (Post ID: " << p->getPostId() << ")\n";
    return p;
}

void Page::attachPost(Post* post) {
    if (post) posts.push_back(post);
}

void Page::deletePost(int postId) {
    for (auto it = posts.begin(); it != posts.end(); ++it) {
        if ((*it)->getPostId() == postId) {
            delete *it;
            posts.erase(it);
            cout << "Post #" << postId << " deleted.\n";
            return;
        }
    }
    cout << "Post #" << postId << " not found.\n";
}

Post* Page::findPost(int postId) const {
    for (Post* p : posts)
        if (p->getPostId() == postId) return p;
    return nullptr;
}

void Page::likePost(int postId, User* user) {
    Post* post = findPost(postId);
    if (!post) { cout << "Post #" << postId << " not found.\n"; return; }
    Like::createLike(user, post);   // Like and Post handle everything
}



void Page::commentOnPost(int postId, User* user, const string& text) {
    Post* post = findPost(postId);
    if (!post) { cout << "Post #" << postId << " not found.\n"; return; }
    Comment* c = new Comment(user, post, text, 500);
    post->addComment(c);
    cout << "@" << user->getUsername()
         << " commented on Post #" << postId << "\n";
}



void Page::showPageInfo() const {
    cout << "\nPage Name : " << pageName        << "\n"
         << "Category  : " << category         << "\n"
         << "Description: " << description     << "\n"
         << "Admin     : " << (admin ? admin->getUsername() : "None") << "\n"
         << "Followers : " << followerCount()  << "\n"
         << "Posts     : " << posts.size()     << "\n";
}

void Page::showTimeline() const {
    cout << "\n===== " << pageName << " Timeline =====\n";
    if (posts.empty()) { cout << "  No posts yet.\n"; return; }
    for (const Post* p : posts) {
        p->viewPost();
        cout << "  Likes: "    << p->getLikeCount()
             << "  Comments: " << p->getComments().size() << "\n"
             << "  ----------------------------------------\n";
    }
}

void Page::showLatestPost() const {
    if (posts.empty()) { cout << "  No posts yet.\n"; return; }
    cout << "\n--- Latest Post ---\n";
    posts.back()->viewPost();
}



int         Page::getPageId()      const { return pageId;      }
string      Page::getPageName()    const { return pageName;    }
string      Page::getDescription() const { return description; }
string      Page::getCategory()    const { return category;    }
User*       Page::getAdmin()       const { return admin;       }
time_t      Page::getCreatedAt()   const { return createdAt;   }
const vector<User*>& Page::getFollowers() const { return followers; }
const vector<Post*>& Page::getPosts()     const { return posts;     }
void Page::setDescription(const string& d) { description = d; }
void Page::setCategory   (const string& c) { category    = c; }
