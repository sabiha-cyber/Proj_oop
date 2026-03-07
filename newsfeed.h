#ifndef NEWSFEED_H
#define NEWSFEED_H

#include <vector>
#include <string>
#include <map>
#include <ctime>
#include "NotificationManager.h"
#include "AuthenticationService.h"
#include "LikeManager.h"
#include "CommentManager.h"
class Post;
class User;
class PostManager;
class FriendService;
class NotificationManager;

class AuthenticationService;

class NewsFeed {
private:
    struct ScoringWeights {
        double timeFactor     = 1.0;
        double likeFactor     = 2.0;
        double commentFactor  = 3.0;
        double friendBonus    = 1.3;
        double categoryBonus  = 1.5;
    };

    std::vector<Post*> feed;
    User* currentUser = nullptr;

    PostManager* postManager = nullptr;
    AuthenticationService* authService = nullptr;
    FriendService* friendService = nullptr;
    NotificationManager* notifMgr = nullptr;
    LikeManager* likeManager;
    CommentManager* commentManager;

    ScoringWeights weights;

    // Helpers
    double calculateScore(Post* post);
    double getDiversityMultiplier(const std::string& category);
    double getHoursDifference(time_t past, time_t present);
    time_t getCurrentTime();
    void collectPosts();
    void applyDiversityFilter();
    std::string getTimeAgo(time_t timestamp);

public:
    NewsFeed(User* user,
         PostManager* pm,
         AuthenticationService* auth,
         FriendService* fs,
         NotificationManager* nm,
         LikeManager* lm,
         CommentManager* cm);

    ~NewsFeed() = default;

    void generateFeed();
    void display(int limit = 20);
    void displayByCategory(const std::string& category, int limit = 20);
    void refresh();

    std::map<std::string, int> getCategoryDistribution();
    void showTrendingCategories();

    void showNewsFeedMenu();
};

#endif
