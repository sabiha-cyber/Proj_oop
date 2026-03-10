#include "PageManager.h"
#include "Page.h"
#include "User.h"
#include "AuthenticationService.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
using namespace std;

const string PageManager::PAGES_FILE = "pages.txt";

PageManager::PageManager()  {}
PageManager::~PageManager() { for (Page* p : pages) delete p; }

// ── Create / Delete ───────────────────────────────────────────────────────────

Page* PageManager::createPage(const string& name, const string& desc,
                               const string& cat, User* admin) {
    if (findByName(name)) {
        cout << "Page '" << name << "' already exists.\n";
        return nullptr;
    }
    Page* p = new Page(name, desc, cat, admin);
    pages.push_back(p);
    saveToFile();
    cout << "Page '" << name << "' created (ID: " << p->getPageId() << ")\n";
    return p;
}

bool PageManager::deletePage(int pageId, User* requestingUser) {
    Page* p = findById(pageId);
    if (!p) { cout << "Page not found.\n"; return false; }
    if (p->getAdmin() && p->getAdmin() != requestingUser) {
        cout << "Only the page admin can delete this page.\n";
        return false;
    }
    pages.erase(find(pages.begin(), pages.end(), p));
    delete p;
    saveToFile();
    cout << "Page deleted.\n";
    return true;
}

// ── Lookup ────────────────────────────────────────────────────────────────────

Page* PageManager::findById(int pageId) const {
    for (Page* p : pages) if (p->getPageId() == pageId) return p;
    return nullptr;
}

Page* PageManager::findByName(const string& name) const {
    for (Page* p : pages) if (p->getPageName() == name) return p;
    return nullptr;
}

vector<Page*> PageManager::getByAdmin(User* admin) const {
    vector<Page*> res;
    for (Page* p : pages) if (p->getAdmin() == admin) res.push_back(p);
    return res;
}

vector<Page*> PageManager::getByCategory(const string& cat) const {
    vector<Page*> res;
    for (Page* p : pages) if (p->getCategory() == cat) res.push_back(p);
    return res;
}

vector<Page*> PageManager::getAllPages() const { return pages; }

// ── Follow ────────────────────────────────────────────────────────────────────

bool PageManager::followPage(User* user, int pageId) {
    Page* p = findById(pageId);
    if (!p) { cout << "Page not found.\n"; return false; }
    bool ok = p->followPage(user);
    if (ok) saveToFile();
    return ok;
}

bool PageManager::unfollowPage(User* user, int pageId) {
    Page* p = findById(pageId);
    if (!p) { cout << "Page not found.\n"; return false; }
    bool ok = p->unfollowPage(user);
    if (ok) saveToFile();
    return ok;
}

// ── Like / Comment on post ────────────────────────────────────────────────────

void PageManager::likePost(int pageId, int postId, User* user) {
    Page* p = findById(pageId);
    if (!p) { cout << "Page not found.\n"; return; }
    p->likePost(postId, user);
}

void PageManager::commentOnPost(int pageId, int postId,
                                 User* user, const string& text) {
    Page* p = findById(pageId);
    if (!p) { cout << "Page not found.\n"; return; }
    p->commentOnPost(postId, user, text);
}

// ── Display ───────────────────────────────────────────────────────────────────

void PageManager::listAllPages() const {
    cout << "\n===== All Pages (" << pages.size() << ") =====\n";
    if (pages.empty()) { cout << "  No pages yet.\n"; return; }
    for (const Page* p : pages)
        cout << "  [" << p->getPageId() << "] "
             << p->getPageName()
             << "  Category: "  << p->getCategory()
             << "  Followers: " << p->followerCount()
             << "  Posts: "     << p->getPosts().size() << "\n";
}

void PageManager::showPageMenu(int pageId, User* me) {
    Page* page = findById(pageId);
    if (!page) { cout << "Page not found.\n"; return; }

    int choice = -1;
    while (choice != 0) {
        cout << "\n=============================\n"
             << " Facebook Page Menu\n"
             << "=============================\n"
             << "1. View Page Info\n"
             << "2. View Timeline\n"
             << "3. Add Post\n"
             << "4. Follow Page\n"
             << "5. View Latest Post\n"
             << "6. Like a Post\n"
             << "7. Comment on a Post\n"
             << "0. Exit\n"
             << "Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {

        case 1: page->showPageInfo();   break;
        case 2: page->showTimeline();   break;

        case 3: {
            cout << "Enter post text: "; string content; getline(cin, content);
            page->createPost(me, content);
            saveToFile();
            break;
        }

        case 4:
            if (page->isFollower(me)) unfollowPage(me, pageId);
            else                       followPage  (me, pageId);
            break;

        case 5: page->showLatestPost(); break;

        case 6: {
            if (page->getPosts().empty()) { cout << "No posts yet.\n"; break; }
            page->showTimeline();
            cout << "Enter post ID to like: ";
            int pid; cin >> pid;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            page->likePost(pid, me);
            break;
        }

        case 7: {
            if (page->getPosts().empty()) { cout << "No posts yet.\n"; break; }
            page->showTimeline();
            cout << "Enter post ID to comment on: ";
            int pid; cin >> pid;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter comment: "; string text; getline(cin, text);
            page->commentOnPost(pid, me, text);
            break;
        }

        case 0: cout << "Exiting...\n"; break;
        default: cout << "Invalid choice\n";
        }
    }
}

// ── Persistence ───────────────────────────────────────────────────────────────
// File format: pageId|pageName|description|category|adminId|createdAt

void PageManager::saveToFile(const string& filename) const {
    ofstream out(filename);
    if (!out) { cout << "Error: cannot write to " << filename << "\n"; return; }
    for (const Page* p : pages)
        out << p->getPageId()                                    << '|'
            << p->getPageName()                                  << '|'
            << p->getDescription()                               << '|'
            << p->getCategory()                                  << '|'
            << (p->getAdmin() ? p->getAdmin()->getUserId() : -1) << '|'
            << static_cast<long long>(p->getCreatedAt())         << '\n';
}

void PageManager::loadFromFile(const string& filename, AuthenticationService& auth) {
    ifstream in(filename);
    if (!in) return;
    for (Page* p : pages) delete p;
    pages.clear();
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string tok[6]; bool ok = true;
        for (int i = 0; i < 6; i++)
            if (!getline(ss, tok[i], '|')) { ok = false; break; }
        if (!ok) continue;
        int    id      = stoi(tok[0]);
        string name    = tok[1], desc = tok[2], cat = tok[3];
        int    adminId = stoi(tok[4]);
        time_t t       = static_cast<time_t>(stoll(tok[5]));
        User*  admin   = auth.findUserById(adminId);
        pages.push_back(new Page(id, name, desc, cat, admin, t));
    }
}