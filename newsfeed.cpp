#include "NewsFeed.h"
#include "FriendService.h"
#include "Post.h"
#include "Page.h"
#include "User.h"
#include "Comment.h"
#include "messenger_manager.h"
#include "messenger_ui.h"
#include "PostManager.h"
#include "UserManager.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <limits>
#include "Like.h"

using namespace std;

NewsFeed::NewsFeed(User* user, PostManager* pm, AuthenticationService* auth,
                   FriendService* fs, NotificationManager* nm,
                   LikeManager* lm, CommentManager* cm)
    : currentUser(user), postManager(pm), authService(auth),
      friendService(fs), notifMgr(nm), likeManager(lm), commentManager(cm) {}

// ---------- scoring helpers ----------
double NewsFeed::calculateScore(Post* post) {
    if (!post || post->isDeletedPost()) return 0.0;

    double score = 0;

    // 1) time score
    double hoursAgo = getHoursDifference(post->getCreationTime(), getCurrentTime());
    double timeScore = 100.0 * exp(-hoursAgo / 48.0);
    score += timeScore * weights.timeFactor;

    // 2) engagement
    double engagementScore = (post->getLikeCount() * weights.likeFactor) +
                             (post->getComments().size() * weights.commentFactor);
    score += engagementScore;

    // 3) friend bonus
    if (currentUser && post->getAuthor() &&
        currentUser->hasFriend(post->getAuthor()->getUserId())) {
        score *= weights.friendBonus;
    }

    // 4) category bonus
    if (currentUser) {
        for (const Post* myPost : currentUser->getPosts()) {
            if (myPost && myPost->getCategory() == post->getCategory()) {
                score *= weights.categoryBonus;
                break;
            }
        }
    }

    // 5) diversity penalty
    score *= getDiversityMultiplier(post->getCategory());

    return score;
}

double NewsFeed::getDiversityMultiplier(const string& category) {
    int sameCategory = 0;
    for (int i = 0; i < min(5, (int)feed.size()); i++) {
        if (feed[i] && feed[i]->getCategory() == category) sameCategory++;
    }
    return 1.0 / (1.0 + sameCategory * 0.2);
}

double NewsFeed::getHoursDifference(time_t past, time_t present) {
    return difftime(present, past) / 3600.0;
}

time_t NewsFeed::getCurrentTime() {
    return time(nullptr);
}

// ---------- feed generation ----------
void NewsFeed::collectPosts() {
    feed.clear();
    if (!postManager || !currentUser) return;

    vector<Post*> allPosts = postManager->getAllActivePosts();

    for (Post* post : allPosts) {
        if (!post || !post->getAuthor()) continue;

        int authorId = post->getAuthor()->getUserId();

        if (authorId == currentUser->getUserId() || currentUser->hasFriend(authorId)) {
            feed.push_back(post);
        }
    }

    // optional: remove older than 7 days
    time_t cutoffTime = getCurrentTime() - (7 * 24 * 3600);
    feed.erase(remove_if(feed.begin(), feed.end(),
                [cutoffTime](Post* p) {
                    return !p || p->getCreationTime() < cutoffTime;
                }),
              feed.end());
}

void NewsFeed::applyDiversityFilter() {
    vector<Post*> balanced;
    string lastCategory;
    int categoryStreak = 0;
    User* lastAuthor = nullptr;
    int authorStreak = 0;

    for (Post* post : feed) {
        if (!post || !post->getAuthor()) continue;
        bool skip = false;

        // category streak
        if (post->getCategory() == lastCategory) {
            categoryStreak++;
            if (categoryStreak >= 3) skip = true;
        } else {
            categoryStreak = 1;
            lastCategory = post->getCategory();
        }

        // author streak
        if (post->getAuthor() == lastAuthor) {
            authorStreak++;
            if (authorStreak >= 2) skip = true;
        } else {
            authorStreak = 1;
            lastAuthor = post->getAuthor();
        }

        if (!skip) balanced.push_back(post);
    }

    feed = balanced;
}

void NewsFeed::generateFeed() {
    collectPosts();
    if (feed.empty()) return;

    sort(feed.begin(), feed.end(),
         [this](Post* a, Post* b) {
            return calculateScore(a) > calculateScore(b);
         });

    applyDiversityFilter();
}

// ---------- display (ASCII only) ----------
void NewsFeed::display(int limit) {
    if (feed.empty()) {
        cout << "\n---------------------------------------------------\n";
        cout << "No posts available in your feed.\n";
        cout << "Add friends to see their posts here!\n";
        cout << "---------------------------------------------------\n";
        return;
    }

    cout << "\n-------------------- NEWS FEED ---------------------\n";
    cout << "Showing " << min(limit, (int)feed.size())
         << " of " << feed.size() << " posts\n";
    cout << "---------------------------------------------------\n\n";

    const size_t maxWidth = 47;

    for (int i = 0; i < min(limit, (int)feed.size()); i++) {
        Post* post = feed[i];
        if (!post || !post->getAuthor()) continue;

        cout << "-- Post #" << (i + 1) << " -----------------------------------------\n";
        cout << "| Author:   " << left << setw(38) << post->getAuthor()->getUsername() << "|\n";
        cout << "| Category: " << left << setw(20) << post->getCategory()
             << " Time: " << left << setw(16) << getTimeAgo(post->getCreationTime()) << "|\n";
        cout << "---------------------------------------------------\n";

        string content = post->getText();
        if (content.length() > maxWidth) {
            for (size_t j = 0; j < content.length(); j += maxWidth) {
                cout << "| " << left << setw((int)maxWidth)
                     << content.substr(j, maxWidth) << "|\n";
            }
        } else {
            cout << "| " << left << setw((int)maxWidth) << content << "|\n";
        }

        cout << "---------------------------------------------------\n";
        cout << "| Likes: " << setw(4) << post->getLikeCount()
             << "  Comments: " << setw(4) << post->getComments().size()
             << "  Shares: " << setw(4) << post->getShareCount() << " |\n";
        cout << "---------------------------------------------------\n\n";
    }
}

void NewsFeed::displayByCategory(const string& category, int limit) {
    cout << "\n---------- Posts in '" << category << "' ----------\n\n";

    int count = 0;
    for (Post* post : feed) {
        if (!post) continue;
        if (post->getCategory() == category && count < limit) {
            post->viewPost();
            cout << "------------------------------------------\n";
            count++;
        }
    }

    if (count == 0) cout << "No posts found in category '" << category << "'\n";
    cout << "\n------------------------------------------\n";
}

void NewsFeed::refresh() {
    cout << "Refreshing feed...\n";
    generateFeed();
    cout << "Feed refreshed with " << feed.size() << " posts.\n";
}

string NewsFeed::getTimeAgo(time_t timestamp) {
    double diff = difftime(getCurrentTime(), timestamp);

    if (diff < 60) return "just now";
    if (diff < 3600) return to_string((int)(diff / 60)) + "m ago";
    if (diff < 86400) return to_string((int)(diff / 3600)) + "h ago";
    if (diff < 604800) return to_string((int)(diff / 86400)) + "d ago";
    return to_string((int)(diff / 604800)) + "w ago";
}

// ---------- analytics ----------
map<string, int> NewsFeed::getCategoryDistribution() {
    map<string, int> distribution;
    for (Post* post : feed) {
        if (!post) continue;
        distribution[post->getCategory()]++;
    }
    return distribution;
}

void NewsFeed::showTrendingCategories() {
    auto dist = getCategoryDistribution();
    if (dist.empty()) {
        cout << "\nNo trending categories yet.\n";
        return;
    }

    cout << "\n---------- Trending Categories ----------\n";

    vector<pair<string, int>> sorted(dist.begin(), dist.end());
    sort(sorted.begin(), sorted.end(),
         [](const auto& a, const auto& b) { return a.second > b.second; });

    int rank = 1;
    for (const auto& [category, count] : sorted) {
        cout << "  " << rank++ << ". " << left << setw(20) << category
             << " (" << count << " posts)\n";
        if (rank > 5) break;
    }
    cout << "----------------------------------------\n";
}

// ---------- main menu ----------
void NewsFeed::showNewsFeedMenu() {
    int choice = -1;

    while (choice != 0) {
        cout << "\n---------------- NEWS FEED MENU ----------------\n";
        cout << "Logged in as: @" << (currentUser ? currentUser->getUsername() : "Unknown") << "\n";
        cout << "1. View News Feed\n";
        cout << "2. Create Post\n";
        cout << "3. Messenger\n";
        cout << "4. My Posts\n";
        cout << "5. Pages (Coming Soon)\n";
        cout << "6. Notifications\n";
        cout << "7. Refresh Feed\n";
        cout << "8. Browse by Category\n";
        cout << "9. Trending Categories\n";
        cout << "10. Friend menu\n";
        cout << "11. Like/Unlike a Post\n";
        cout << "12. Comment on a Post\n";
        cout << "0. Back\n";
        cout << "Choice: ";

        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:
                generateFeed();
                display(20);
                break;

            case 2: {
                if (!postManager || !currentUser) break;
                Post* newPost = postManager->createAndAdd(currentUser);
                if (newPost) {
                    currentUser->addPost(newPost);
                    cout << "Post added to your profile!\n";
                }
                break;
            }

            case 3: {
                MessengerManager messengerMgr;
                for (User& u : authService->getUsers()) {
                    messengerMgr.addUser(&u);
                }
                messengerMgr.loadDatabase();
                messengerMgr.login(to_string(currentUser->getUserId()));
                MessengerUI messengerUI(messengerMgr);
                messengerUI.runChatInterface();
                break;
            }

            case 4:
                if (currentUser) currentUser->showMyPosts();
                break;

            case 5:
                cout << "Pages feature coming soon!\n";
                break;

            case 6:
                if (notifMgr && currentUser) notifMgr->showInbox(currentUser->getUserId());
                else cout << "Notifications system not connected.\n";
                break;

            case 7:
                refresh();
                break;

            case 8: {
                string category;
                cout << "Enter category name: ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, category);
                generateFeed();
                displayByCategory(category, 10);
                break;
            }

            case 9:
                generateFeed();
                showTrendingCategories();
                break;

            case 10: {
                if (!friendService || !authService || !currentUser) {
                    cout << "Friend system not connected.\n";
                    break;
                }

                int friendChoice = -1;
                while (friendChoice != 0) {
                    cout << "\n--------- FRIEND MENU ---------\n";
                    cout << "1. Send Friend Request\n";
                    cout << "2. Accept Friend Request\n";
                    cout << "3. Reject Friend Request\n";
                    cout << "4. View Pending Requests\n";
                    cout << "5. View My Friends\n";
                    cout << "6. Remove Friend\n";
                    cout << "7. View All Users\n";
                    cout << "0. Back\n";
                    cout << "Choice: ";
                    cin >> friendChoice;

                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid input.\n";
                        friendChoice = -1;
                        continue;
                    }

                    switch (friendChoice) {
                        case 1: {
                            authService->listAllUsers();
                            string targetUsername;
                            cout << "Enter username to send request to: ";
                            cin >> targetUsername;
                            friendService->sendFriendRequest(currentUser->getUserId(), targetUsername);
                            break;
                        }

                        case 2: {
                            friendService->showPendingRequestsForUser(currentUser->getUserId());
                            int senderId;
                            cout << "Enter sender ID to accept (0 to cancel): ";
                            cin >> senderId;
                            if (senderId != 0)
                                friendService->acceptFriendRequest(currentUser->getUserId(), senderId);
                            break;
                        }

                        case 3: {
                            friendService->showPendingRequestsForUser(currentUser->getUserId());
                            int senderId;
                            cout << "Enter sender ID to reject (0 to cancel): ";
                            cin >> senderId;
                            if (senderId != 0)
                                friendService->rejectFriendRequest(currentUser->getUserId(), senderId);
                            break;
                        }

                        case 4:
                            friendService->showPendingRequestsForUser(currentUser->getUserId());
                            break;

                        case 5: {
                            vector<int> friendIds = friendService->getFriendIdsOf(currentUser->getUserId());
                            if (friendIds.empty()) {
                                cout << "You have no friends yet.\n";
                            } else {
                                cout << "\nMy friends:\n";
                                for (int fid : friendIds) {
                                    User* u = authService->findUserById(fid);
                                    if (u) cout << "  ID: " << fid << " | @" << u->getUsername() << "\n";
                                }
                            }
                            break;
                        }

                        case 6: {
                            vector<int> friendIds = friendService->getFriendIdsOf(currentUser->getUserId());
                            if (friendIds.empty()) {
                                cout << "You have no friends to remove.\n";
                                break;
                            }
                            cout << "\nMy friends:\n";
                            for (int fid : friendIds) {
                                User* u = authService->findUserById(fid);
                                if (u) cout << "  ID: " << fid << " | @" << u->getUsername() << "\n";
                            }
                            int removeId;
                            cout << "Enter User ID to remove: ";
                            cin >> removeId;
                            friendService->removeFriend(currentUser->getUserId(), removeId);
                            break;
                        }

                        case 7:
                            authService->listAllUsers();
                            break;

                        case 0:
                            break;

                        default:
                            cout << "Invalid choice.\n";
                    }
                }
                break;
            }
            case 11: {
                generateFeed();
                if (feed.empty()) { cout << "No posts to like.\n"; break; }

                for (int i = 0; i < min(10, (int)feed.size()); i++) {
                    string preview = feed[i]->getText();
                    if (preview.length() > 40) preview = preview.substr(0, 40) + "...";
                    cout << "  " << i + 1 << ". @" << feed[i]->getAuthor()->getUsername()
                        << ": " << preview << " [" << feed[i]->getLikeCount() << " likes]\n";
                }

                int postNum;
                cout << "Enter post number (0 to cancel): ";
                cin >> postNum;
                if (postNum < 1 || postNum > (int)feed.size()) break;

                Post* post = feed[postNum - 1];

                // Check if already liked by this user
                bool alreadyLiked = false;
                for (Like* l : post->getLikes()) {
                    if (l && l->getUser() &&
                        l->getUser()->getUserId() == currentUser->getUserId()) {
                        alreadyLiked = true;
                        break;
                    }
                }

                if (alreadyLiked) {
                    likeManager->removeLike(currentUser, post);
                    cout << "Like removed.\n";
                } else {
                    Like* like = likeManager->createAndAdd(currentUser, post);
                    if (like) {
                        cout << "Post liked!\n";
                        if (notifMgr &&
                            post->getAuthor()->getUserId() != currentUser->getUserId()) {
                            notifMgr->notifyLike(
                                post->getAuthor()->getUserId(),
                                currentUser->getUserId(),
                                currentUser->getUsername(),
                                post->getPostId()
                            );
                        }
                    }
                }
                likeManager->saveToFile("likes.txt");
                break;
            }

            case 12: {
                generateFeed();
                if (feed.empty()) { cout << "No posts to comment on.\n"; break; }

                for (int i = 0; i < min(10, (int)feed.size()); i++) {
                    string preview = feed[i]->getText();
                    if (preview.length() > 40) preview = preview.substr(0, 40) + "...";
                    cout << "  " << i + 1 << ". @" << feed[i]->getAuthor()->getUsername()
                        << ": " << preview
                        << " [" << feed[i]->getComments().size() << " comments]\n";
                }

                int postNum;
                cout << "Enter post number (0 to cancel): ";
                cin >> postNum;
                if (postNum < 1 || postNum > (int)feed.size()) break;

                Post* post = feed[postNum - 1];

                // Show existing comments
                if (!post->getComments().empty()) {
                    cout << "\n--- Comments ---\n";
                    for (Comment* c : post->getComments()) {
                        if (c && !c->isDeletedComment())
                            cout << "  @" << c->getAuthor()->getUsername()
                                << ": " << c->getText() << "\n";
                    }
                    cout << "----------------\n";
                }

                // Get comment text from user
                string commentText;
                cout << "Enter your comment (empty to cancel): ";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, commentText);
                if (commentText.empty()) break;

                // Create comment directly without re-prompting
                int newId = (int)commentManager->getAllComments().size() + 1;
                Comment* comment = new Comment(newId, currentUser, post,
                                            commentText, 500,
                                            time(nullptr), false);
                post->addComment(comment);
                commentManager->addComment(comment);
                cout << "Comment posted!\n";

                if (notifMgr &&
                    post->getAuthor()->getUserId() != currentUser->getUserId()) {
                    notifMgr->notifyComment(
                        post->getAuthor()->getUserId(),
                        currentUser->getUserId(),
                        currentUser->getUsername(),
                        post->getPostId(),
                        commentText
                    );
                }
                commentManager->saveToFile("comments.txt");
                break;
            }
            case 0:
                cout << "Returning...\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }
    }
}
