#include "PostManager.h"
#include "Post.h"
#include "User.h"
#include "LikeManager.h"
#include "CommentManager.h"
#include "SocialExceptions.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

// ── Constructor / Destructor ──────────────────────────────────────────────────

PostManager::PostManager(size_t capacity)
    : defaultCapacity(capacity), nextPostId(1)
{}

PostManager::~PostManager() {
    // Base class destructor handles deleting items
}

// ── Core ──────────────────────────────────────────────────────────────────────

void PostManager::addPost(Post* p) {
    if (!p) {
        cout << "Error: null post.\n";
        return;
    }
    if (p->getPostId() >= nextPostId) {
        nextPostId = p->getPostId() + 1;
    }
    addItem(p);
}

Post* PostManager::createAndAdd(User* user) {
    if (!user) {
        cout << "Error: null user.\n";
        return nullptr;
    }
    Post* p = Post::createPost(nextPostId, user, defaultCapacity);
    if (p) {
        addItem(p);
        ++nextPostId;
    }
    return p;
}

Post* PostManager::createAndAdd(User* user, const string& content, const string& cat) {
    if (!user) {
        cout << "Error: null user.\n";
        return nullptr;
    }
    Post* p = new Post(nextPostId, user, content, defaultCapacity, cat);
    addItem(p);
    ++nextPostId;
    return p;
}

// ── Lookup ────────────────────────────────────────────────────────────────────

Post* PostManager::findPostById(int id) const {
    for (Post* p : getItems()) {
        if (p->getPostId() == id) return p;
    }
    return nullptr;
}

vector<Post*> PostManager::getAllActivePosts() const {
    vector<Post*> result;
    for (Post* p : getItems()) {
        if (!p->isDeletedPost()) result.push_back(p);
    }
    return result;
}

vector<Post*> PostManager::getPostsByUser(User* user) const {
    vector<Post*> result;
    for (Post* p : getItems()) {
        if (p->getAuthor() == user && !p->isDeletedPost())
            result.push_back(p);
    }
    return result;
}

vector<Post*> PostManager::getPostsByCategory(const string& cat) const {
    vector<Post*> result;
    for (Post* p : getItems()) {
        if (p->getCategory() == cat && !p->isDeletedPost())
            result.push_back(p);
    }
    return result;
}

// ── Deletion ──────────────────────────────────────────────────────────────────

void PostManager::deletePostById(int id, User* requestingUser,
                                  CommentManager& cm, LikeManager& lm) {
    Post* post = findPostById(id);

    if (!post) {
        cout << "Post #" << id << " not found.\n";
        return;
    }

    if (post->getAuthor() != requestingUser) {
        cout << "Permission denied: you can only delete your own posts.\n";
        return;
    }

    if (post->isDeletedPost()) {
        cout << "Post #" << id << " is already deleted.\n";
        return;
    }


    cm.removeCommentsForPost(post);   // 1. delete Comment* from CommentManager
    lm.removeLikesForPost(post);      // 2. delete Like* from LikeManager
    post->deletePost();               // 3. clears post's internal vectors, sets flag

    cout << "Post #" << id << " and all its interactions removed.\n";
}

// ── Persistence ───────────────────────────────────────────────────────────────
/*
    Format: postId|authorUsername|category|capacity|shareCount|createdAt|content
    Deleted posts are skipped — not worth persisting.
*/

void PostManager::saveToFile(const string& filename) const {
    try {
        ofstream out(filename);
        if (!out) throw FileIOException("Cannot open file: " + filename);

        int saved = 0;
        for (const Post* p : getItems()) {
            if (p->isDeletedPost()) continue;

            out << p->getPostId()                                         << '|'
                << (p->getAuthor() ? p->getAuthor()->getUsername() : "") << '|'
                << p->getCategory()                                       << '|'
                << p->getMaxCapacity()                                    << '|'
                << p->getShareCount()                                     << '|'
                << p->getCreationTime()                                   << '|'
                << p->getText()                                           << '\n';
            ++saved;
        }

        cout << saved << " post(s) saved to '" << filename << "'.\n";
    } catch (const FileIOException& e) {
        std::cerr << "File error: " << e.what() << "\n";
        // Continue without saving—core logic unchanged
    }
}

void PostManager::loadFromFile(const string& filename, vector<User*>& users) {
    try {
        ifstream in(filename);
        if (!in) throw FileIOException("Cannot open file: " + filename);

        string line;
        int loaded = 0;

        while (getline(in, line)) {
            if (line.empty()) continue;

            istringstream ss(line);
            string idStr, authorName, cat, capStr, shareStr, timeStr, content;

            if (!getline(ss, idStr,      '|') ||
                !getline(ss, authorName, '|') ||
                !getline(ss, cat,        '|') ||
                !getline(ss, capStr,     '|') ||
                !getline(ss, shareStr,   '|') ||
                !getline(ss, timeStr,    '|') ||
                !getline(ss, content         ))
            {
                cout << "Warning: Skipping malformed line.\n";
                continue;
            }

            // Resolve username → User*
            User* author = nullptr;
            for (User* u : users) {
                if (u->getUsername() == authorName) {
                    author = u;
                    break;
                }
            }

            if (!author) {
                cout << "Warning: Author '" << authorName << "' not found. Skipping.\n";
                continue;
            }

            size_t cap = static_cast<size_t>(stoul(capStr));
            int    pid = stoi(idStr);
            Post*  p   = new Post(pid, author, content, cap, cat);

            addItem(p);
            author->addPost(p);   // attach to user's "My Posts" list
            if (pid >= nextPostId) nextPostId = pid + 1;
            ++loaded;
        }

        cout << loaded << " post(s) loaded from '" << filename << "'.\n";
    } catch (const FileIOException& e) {
        std::cerr << "File error: " << e.what() << "\n";
        // Continue without loading—core logic unchanged, starting fresh
    }
}

// ── Getters ───────────────────────────────────────────────────────────────────

int PostManager::getNextPostId() const { return nextPostId; }