#include "LikeManager.h"

#include "Like.h"
#include "Post.h"
#include "User.h"
#include "PostManager.h"
#include "UserManager.h"

#include <fstream>

using namespace std;

LikeManager::~LikeManager() {
    clear();
}

void LikeManager::addLike(Like* like) {
    if (like) likes.push_back(like);
}

Like* LikeManager::createAndAdd(User* user, Post* post) {
    if (!user || !post) return nullptr;

    // Uses Like::createLike which also attaches to Post
    Like* l = Like::createLike(user, post);
    if (l) likes.push_back(l);
    return l;
}

bool LikeManager::removeLike(User* user, Post* post) {
    if (!user || !post) return false;

    for (auto it = likes.begin(); it != likes.end(); ++it) {
        Like* l = *it;
        if (l && l->getUser() == user && l->getPost() == post) {
            post->removeLike(l); // detach first (prevents dangling Like* in Post)
            delete l;
            likes.erase(it);
            return true;
        }
    }
    return false;
}

void LikeManager::removeLikesForPost(Post* post) {
    if (!post) return;

    for (auto it = likes.begin(); it != likes.end(); ) {
        Like* l = *it;
        if (l && l->getPost() == post) {
            post->removeLike(l); // detach first
            delete l;
            it = likes.erase(it);
        } else {
            ++it;
        }
    }
}

Like* LikeManager::findLikeById(int id) const {
    for (Like* l : likes) {
        if (l && l->getLikeId() == id) return l;
    }
    return nullptr;
}

const vector<Like*>& LikeManager::getAllLikes() const {
    return likes;
}

void LikeManager::clear() {
    for (Like* l : likes) {
        if (!l) continue;

        // detach if post still exists
        if (Post* p = l->getPost()) {
            p->removeLike(l);
        }

        delete l;
    }
    likes.clear();
}

// Format: likeId postId userId createdAt
void LikeManager::saveToFile(const string& filename) const {
    ofstream out(filename);
    if (!out) return;

    for (const Like* l : likes) {
        if (!l || !l->getPost() || !l->getUser()) continue;
        if (l->getPost()->isDeletedPost()) continue;

        out << l->getLikeId() << ' '
            << l->getPost()->getPostId() << ' '
            << l->getUser()->getUserId() << ' '
            << static_cast<long long>(l->getTime())
            << '\n';
    }
}

void LikeManager::loadFromFile(const string& filename,
                               PostManager& postManager,
                               UserManager& userManager) {
    ifstream in(filename);
    if (!in) return;

    clear();

    int likeId, postId, userId;
    long long t;

    while (in >> likeId >> postId >> userId >> t) {
        Post* post = postManager.findPostById(postId);
        User* user = userManager.findUserById(userId);

        if (!post || !user) continue;
        if (post->isDeletedPost()) continue;

        // Prevent duplicate likes for same user+post
        bool alreadyLiked = false;
        for (Like* existing : post->getLikes()) {
            if (existing && existing->getUser() == user) {
                alreadyLiked = true;
                break;
            }
        }
        if (alreadyLiked) continue;

        Like* l = new Like(likeId, user, post, static_cast<time_t>(t));
        post->addLike(l);
        likes.push_back(l);
    }
}
