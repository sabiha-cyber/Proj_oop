#include "CommentManager.h"

#include "Comment.h"
#include "Post.h"
#include "User.h"
#include "PostManager.h"
#include "UserManager.h"

#include <fstream>
#include <iomanip>   // std::quoted

using namespace std;

CommentManager::~CommentManager() {
    clear();
}

void CommentManager::addComment(Comment* comment) {
    if (comment != nullptr) {
        comments.push_back(comment);
    }
}

void CommentManager::clear() {
    for (Comment* c : comments) {
        delete c;
    }
    comments.clear();
}

// Hard-delete all comments belonging to a specific post (used when deleting a post)
void CommentManager::removeCommentsForPost(Post* post) {
    if (post == nullptr) return;

    for (auto it = comments.begin(); it != comments.end(); ) {
        Comment* c = *it;

        if (c != nullptr && c->getPost() == post) {
            delete c;
            it = comments.erase(it);
        } else {
            ++it;
        }
    }
}

// Soft delete (safe because Post still holds Comment* in its vector)
bool CommentManager::deleteCommentById(int id) {
    Comment* c = findCommentById(id);
    if (c == nullptr) return false;

    c->deleteComment(); // sets isDeleted + replaces text
    return true;
}

Comment* CommentManager::findCommentById(int id) const {
    for (Comment* c : comments) {
        if (c != nullptr && c->getCommentId() == id) {
            return c;
        }
    }
    return nullptr;
}

const vector<Comment*>& CommentManager::getAllComments() const {
    return comments;
}

// File format (one comment per line):
// commentId postId userId capacity deletedFlag createdAt "text..."
void CommentManager::saveToFile(const string& filename) const {
    ofstream out(filename);
    if (!out) return;

    for (const Comment* c : comments) {
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

void CommentManager::loadFromFile(const string& filename,
                                 PostManager& postManager,
                                 UserManager& userManager) {
    ifstream in(filename);
    if (!in) return;

    // Best used at program startup (before posts have any comment pointers).
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

        // If the post is deleted, don't re-attach comments to it
        if (post->isDeletedPost()) continue;

        Comment* c = new Comment(commentId, user, post, text,
                                 cap,
                                 static_cast<time_t>(t),
                                 deletedFlag != 0);

        comments.push_back(c);
        post->addComment(c);
    }
}
