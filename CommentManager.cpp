#include "CommentManager.h"
#include "Post.h"
#include "User.h"
#include "PostManager.h"
#include "UserManager.h"
#include "AuthenticationService.h"
#include <fstream>
#include <iomanip>     // std::quoted

using namespace std;

CommentManager::~CommentManager() {
    clear(); // detach from posts + delete comments safely
}

void CommentManager::addComment(Comment* comment) {
    if (!comment) return;
    addItem(comment); // GenericManager<Comment>::addItem -> pushes into items
}

void CommentManager::clear() {
    // Detach from posts before deleting to avoid Post holding dangling pointers
    for (Comment* c : items) {
        if (!c) continue;

        if (Post* p = c->getPost()) {
            p->removeCommentPointer(c);
        }
        delete c;
    }
    items.clear();
}

void CommentManager::removeCommentsForPost(Post* post) {
    if (!post) return;

    for (auto it = items.begin(); it != items.end(); ) {
        Comment* c = *it;

        if (c && c->getPost() == post) {
            post->removeCommentPointer(c); // detach first
            delete c;                      // hard delete
            it = items.erase(it);
        } else {
            ++it;
        }
    }
}

bool CommentManager::deleteCommentById(int id) {
    Comment* c = findCommentById(id);
    if (!c) return false;

    c->deleteComment(); //Soft delete
    return true;
}

Comment* CommentManager::findCommentById(int id) const {
    for (Comment* c : items) {
        if (c && c->getCommentId() == id)
            return c;
    }
    return nullptr;
}

void CommentManager::saveToFile(const std::string& filename) const {
    ofstream out(filename);
    if (!out) return;

    // Format (one per line):
    // commentId postId userId capacity deletedFlag createdAt "text..."
    for (const Comment* c : items) {
        if (!c || !c->getPost() || !c->getAuthor()) continue;

        int commentId   = c->getCommentId();
        int postId      = c->getPost()->getPostId();
        int userId      = c->getAuthor()->getUserId();
        size_t cap      = c->getMaxCapacity();
        int deletedFlag = c->isDeletedComment() ? 1 : 0;
        long long t     = static_cast<long long>(c->getCreationTime());

        out << commentId << ' '
            << postId << ' '
            << userId << ' '
            << cap << ' '
            << deletedFlag << ' '
            << t << ' '
            << quoted(c->getText())
            << '\n';
    }
}

void CommentManager::loadFromFile(const std::string& filename,
                                 PostManager& postManager,
                                 UserManager& userManager) {
    ifstream in(filename);
    if (!in) return;

    clear();

    int commentId, postId, userId;
    size_t cap;
    int deletedFlag;
    long long t;
    string text;

    while (in >> commentId >> postId >> userId >> cap >> deletedFlag >> t >> quoted(text)) {
        Post* post = postManager.findPostById(postId);
        User* user = userManager.findUserById(userId);
        if (!post || !user) continue;

        if (post->isDeletedPost()) continue;

        Comment* c = new Comment(commentId, user, post, text,
                                 cap,
                                 static_cast<time_t>(t),
                                 deletedFlag != 0);

        addItem(c);
        post->addComment(c);
    }
}
void CommentManager::loadFromFile(const std::string& filename,
                                 PostManager& postManager,
                                 AuthenticationService& authService){
    std::ifstream in(filename);
    if (!in) return;

    clear();

    int commentId, postId, userId;
    size_t cap;
    int deletedFlag;
    long long t;
    std::string text;

    while (in >> commentId >> postId >> userId >> cap >> deletedFlag >> t >> std::quoted(text)) {
        Post* post = postManager.findPostById(postId);
        User* user = authService.findUserById(userId);

        if (!post || !user) continue;
        if (post->isDeletedPost()) continue;

        Comment* c = new Comment(commentId, user, post, text,
                                 cap,
                                 static_cast<time_t>(t),
                                 deletedFlag != 0);

        addItem(c);
        post->addComment(c); // makes counts/showComments work
    }
}